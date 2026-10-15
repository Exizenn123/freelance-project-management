#!/usr/bin/env python3
"""Runs the REAL household executable in a pseudo-terminal (so typed input is echoed like a
real console), records the true output, checks it against the expected text, and renders each
transcript to Tests/Screenshots/Txx_PASS.png.  Usage:  python3 Tests/run_tests.py"""
import os, pty, select, subprocess, shutil, json, tempfile, re, sys, time
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
EXE = os.path.join(ROOT, "household")
SHOTS = os.path.join(HERE, "Screenshots"); os.makedirs(SHOTS, exist_ok=True)
ANSI = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")

def session(inputs, cwd):
    """Drive the program through a pty: send each line only after the program has printed its prompt."""
    pid, fd = pty.fork()
    if pid == 0:
        os.chdir(cwd); os.execv(EXE, [EXE])
    buf = b""
    def drain(t):
        nonlocal buf
        while True:
            r, _, _ = select.select([fd], [], [], t)
            if not r: return True
            try: d = os.read(fd, 4096)
            except OSError: return False
            if not d: return False
            buf += d
    alive = drain(0.25)
    for line in inputs:
        if not alive: break
        os.write(fd, (line + "\n").encode()); alive = drain(0.06)
    drain(0.4)
    os.waitpid(pid, 0)
    return ANSI.sub("", buf.decode(errors="replace").replace("\r\n", "\n").replace("\r", ""))

def last_lines(text, key):
    """Return the real output line (from the transcript) that contains the key."""
    for l in text.splitlines():
        if key in l: return l.strip()
    return None

def row_c(i, n, ph): return f"{i:<5}{n:<22}{ph}"
SETUP = ["1","1","Neha Joshi","9876543210","1","1","Rohit Verma","9123456780",
         "1","2","Karan Mehta","Web Design","800","1","2","Pooja Nair","Data Entry","300"]
PROJ = ["6","1","1","Company Website","20000"]
ASSIGN = ["6","2","1","1"]
PAY5 = ["6","3","1","5000","20-10-2026"]
PAYALL = ["6","3","1","15000","25-10-2026"]
T = [
 ("T01","Add Client","Name=Neha Joshi, Phone=9876543210","Client added with ID 1",["1","1","Neha Joshi","9876543210","9"]),
 ("T02","Add Client (invalid phone)","Phone=12345","Error: Phone must be exactly 10 digits",["1","1","Neha Joshi","12345","9"]),
 ("T03","Add Freelancer","Karan Mehta, Web Design, Rs. 800/hr","Freelancer added with ID 1",["1","2","Karan Mehta","Web Design","800","9"]),
 ("T04","Add Freelancer (negative rate)","Rate=-50","Error: Rate must be between 1 and 100000",["1","2","Karan Mehta","Web Design","-50","9"]),
 ("T05","Create Project","Client 1, 'Company Website', Rs. 20000","Project created with ID 1 (status Open)",SETUP+PROJ+["9"]),
 ("T06","Create Project (unknown client)","Client ID 9","Error: Client not found",SETUP+["6","1","9","Logo","1000","9"]),
 ("T07","Create Project (zero budget)","Budget=0","Error: Budget must be between 1 and 10000000",SETUP+["6","1","1","Logo","0","9"]),
 ("T08","Display Clients","Display > Clients",row_c(2,"Rohit Verma","9123456780"),SETUP+["2","1","9"]),
 ("T09","Search Freelancer by skill","Skill contains 'web' (case-insensitive)",f"{1:<5}{'Karan Mehta':<22}{'Web Design':<18}Rs. 800.00/hr",SETUP+["3","2","web","9"]),
 ("T10","Update Freelancer rate","Freelancer 1 -> Rs. 950/hr","Freelancer updated.",SETUP+["4","2","1","950","9"]),
 ("T11","Delete Client without projects","Delete client 2","Client deleted.",SETUP+["5","1","2","9"]),
 ("T12","Delete blocked: client has a project","Delete client 1","Cannot delete: client has a project.",SETUP+PROJ+["5","1","1","9"]),
 ("T13","Assign Freelancer","Project 1 -> Freelancer 1","Freelancer assigned. Project 1 is now Assigned.",SETUP+PROJ+ASSIGN+["9"]),
 ("T14","Assign blocked: freelancer busy","Freelancer 1 on a 2nd project","Freelancer is already working on project 1",SETUP+PROJ+ASSIGN+["6","1","1","Logo Design","3000","6","2","2","1","9"]),
 ("T15","Record Payment","Project 1, Rs. 5000, 20-10-2026","Payment recorded. Payment ID 1. Paid Rs. 5000.00 of Rs. 20000.00",SETUP+PROJ+ASSIGN+PAY5+["9"]),
 ("T16","Payment exceeds budget","Rs. 25000 on a Rs. 20000 project","Error: Payment exceeds the budget (balance Rs. 20000.00)",SETUP+PROJ+ASSIGN+["6","3","1","25000","20-10-2026","9"]),
 ("T17","Payment (invalid date)","Date=31-02-2026","Error: Date must be a valid DD-MM-YYYY",SETUP+PROJ+ASSIGN+["6","3","1","5000","31-02-2026","9"]),
 ("T18","Close blocked: unpaid balance","Close with Rs. 5000 of 20000 paid","Error: Cannot close: Rs. 15000.00 still unpaid",SETUP+PROJ+ASSIGN+PAY5+["6","4","1","9"]),
 ("T19","Pay in full, then Close","Pay remaining 15000, close project 1","Project closed. Total paid Rs. 20000.00",SETUP+PROJ+ASSIGN+PAY5+PAYALL+["6","4","1","9"]),
 ("T20","Cancel blocked: project has payments","Cancel project 1 after a payment","Error: Cannot cancel: project already has payments",SETUP+PROJ+ASSIGN+PAY5+["6","5","1","9"]),
 ("T21","Cancel Open Project","Cancel project 1","Project cancelled.",SETUP+PROJ+["6","5","1","9"]),
 ("T22","Polymorphic display (Person*)","Display > Everyone","Freelancer: "+f"{1:<5}{'Karan Mehta':<22}",SETUP+["2","5","9"]),
 ("T23","Estimate fee (calculateFee)","Freelancer 1, 5 hours","Estimated fee for Karan Mehta: Rs. 4000.00",SETUP+["6","6","1","5","9"]),
 ("T24","Report with earnings per freelancer","Project paid Rs. 5000, then Report","Karan Mehta"+" "*11+"Rs. 5000.00",SETUP+PROJ+ASSIGN+PAY5+["7","9"]),
]

def font(sz, bold=False):
    for p in ["/usr/share/fonts/truetype/dejavu/DejaVuSansMono%s.ttf" % ("-Bold" if bold else ""),
              "/usr/share/fonts/truetype/liberation/LiberationMono-%s.ttf" % ("Bold" if bold else "Regular")]:
        if os.path.exists(p): return ImageFont.truetype(p, sz)
    return ImageFont.load_default()

def render(tid, title, status, transcript, inputs, expected, path):
    """Terminal-style PNG of the real transcript; typed input shown in cyan, the expected result line highlighted."""
    lines = transcript.splitlines()
    # show from the first interaction of the TEST (skip long set-up menus): keep last 34 lines
    omitted = max(0, len(lines) - 34); lines = lines[omitted:]
    f, fb = font(17), font(17, True)
    lh, pad, w = 24, 26, 1180
    h = pad*2 + 46 + lh*(len(lines)+ (1 if omitted else 0)) + 40
    im = Image.new("RGB", (w, h), (18, 18, 20)); d = ImageDraw.Draw(im)
    d.rectangle([0, 0, w, 40], fill=(48, 48, 54))
    for i, c in enumerate([(255,95,86),(255,189,46),(39,201,63)]): d.ellipse([16+i*26, 12, 30+i*26, 26], fill=c)
    d.text((110, 9), f"{tid} - {title}   |   household  (IU2541230786 - Divyansh Jogai)", font=f, fill=(210,210,215))
    badge = "PASS" if status == "PASS" else "FAIL"
    d.rounded_rectangle([w-110, 6, w-14, 34], 6, fill=(30,150,60) if status=="PASS" else (190,40,40))
    d.text((w-98, 10), badge, font=fb, fill=(255,255,255))
    y = 40 + pad
    if omitted: d.text((pad, y), f"... ({omitted} earlier lines omitted) ...", font=f, fill=(120,120,130)); y += lh
    echoed = set(inputs)
    for ln in lines:
        hit = expected.split("\n")[0] in ln
        if hit: d.rectangle([pad-6, y-2, w-pad+6, y+lh-4], fill=(30, 70, 40))
        # split prompt (grey/white) from typed echo (cyan): typed text = trailing part after last ': '
        m = re.match(r"^(.*?: )(\S.*)$", ln)
        if m and m.group(2).strip() in echoed and not hit and len(ln) < 70 and ("Enter choice" in ln or "Choice" in ln or ": " in ln):
            d.text((pad, y), m.group(1), font=f, fill=(235,235,235))
            d.text((pad + d.textlength(m.group(1), font=f), y), m.group(2), font=fb, fill=(80,220,235))
        else:
            d.text((pad, y), ln, font=f, fill=(120,255,150) if hit else (235,235,235))
        y += lh
    d.text((pad, h-34), f"Expected: \"{expected}\"   ->   {status}", font=f, fill=(255,214,102))
    im.save(path)

def main():
    if not os.path.exists(EXE): sys.exit("Build first: g++ -std=c++17 -Wall -Wextra -o household src/*.cpp")
    results = []
    for tid, op, inp, exp, inputs in T:
        d = tempfile.mkdtemp(); out = session(inputs, d)
        actual = last_lines(out, exp.split("\n")[0]); ok = actual is not None
        res = dict(id=tid, op=op, input=inp, expected=exp, actual=actual or "(expected text NOT found)", status="PASS" if ok else "FAIL", out=out)
        results.append(res); shutil.rmtree(d)
        render(tid, op, res["status"], out, inputs, exp, os.path.join(SHOTS, f"{tid}_{res['status']}.png"))
    # T25 persistence: run 1 saves on exit, run 2 (fresh process, same Data folder) reloads
    d = tempfile.mkdtemp(); session(SETUP + PROJ + ASSIGN + PAY5 + ["9"], d)
    in2 = ["7","9"]; out2 = session(in2, d)
    exp = "Data loaded: 2 clients, 2 freelancers, 1 projects, 1 payments."
    ok = exp in out2 and "Assigned 1" in out2 and "Total paid: Rs. 5000.00" in out2
    r = dict(id="T25", op="Persistence: save on exit, reload on restart", input="Run 1: add data, create, assign, pay, Exit. Run 2: restart, Report",
             expected=exp + " + Assigned 1, paid Rs. 5000.00",
             actual=(last_lines(out2, "Data loaded") + " + " + last_lines(out2, "Projects :") + " + " + last_lines(out2, "Total paid")) if ok else "(mismatch)",
             status="PASS" if ok else "FAIL", out=out2)
    results.append(r); shutil.rmtree(d)
    render("T25", r["op"], r["status"], out2, in2, exp, os.path.join(SHOTS, f"T25_{r['status']}.png"))
    results.sort(key=lambda r: r["id"])
    json.dump(results, open(os.path.join(HERE, "results.json"), "w"), indent=1)
    for r in results: print(r["id"], r["status"], "-", r["op"], "|", r["actual"])
    print("TOTAL: %d/%d passed" % (sum(r["status"] == "PASS" for r in results), len(results)))
    return 0 if all(r["status"] == "PASS" for r in results) else 1
if __name__ == "__main__": sys.exit(main())
