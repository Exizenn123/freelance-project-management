import subprocess, matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# ---------- Class diagram (graphviz) ----------
cls = r'''digraph G { rankdir=BT; node [shape=record, fontname="Helvetica", fontsize=10]; edge [fontname="Helvetica", fontsize=9];
Person [label="{«abstract»\nPerson|# id : int\l# name : string\l|+ Person()\l+ Person(id, name)\l+ ~Person() «virtual»\l+ getId() : int\l+ getName() : string\l+ setName(n) : void\l+ display() : void «pure virtual»\l+ toRecord() : string «pure virtual»\l+ role() : string «pure virtual»\l}"];
Client [label="{Client|- phone : string\l|+ Client()\l+ Client(id, name, phone)\l+ ~Client()\l+ getPhone() : string\l+ setPhone(p) : void\l+ display() : void\l+ toRecord() : string\l+ role() : string\l}"];
Freelancer [label="{Freelancer|- skill : string\l- rate : double\l|+ Freelancer()\l+ Freelancer(id, name, skill, rate)\l+ ~Freelancer()\l+ setSkill(s) : void\l+ setRate(r) : void\l+ calculateFee(hours) : double\l+ display() : void\l+ toRecord() : string\l+ role() : string\l}"];
Project [label="{Project|- projectId : int\l- clientId : int\l- freelancerId : int\l- title : string\l- budget : double\l- status : Status\l|+ Project()\l+ Project(id, clientId, title, budget)\l+ ~Project()\l+ setTitle(t) : void\l+ setBudget(b) : void\l+ assign(freelancerId) : void\l+ updateStatus(next) : void\l+ display() : void\l+ toRecord() : string\l}"];
Payment [label="{Payment|- paymentId : int\l- projectId : int\l- amount : double\l- date : string\l|+ Payment()\l+ Payment(id, projectId, amount, date)\l+ ~Payment()\l+ makePayment(id, projectId, amount, date) : void\l+ display() : void\l+ toRecord() : string\l}"];
FileManager [label="{FileManager|- dir : string\l|+ FileManager(dir)\l+ ~FileManager()\l+ save(...) : void\l+ load(...) : int\l}"];
FS [label="{FreelanceSystem|- clients : Client*\l- freelancers : Freelancer*\l- projects : Project*\l- payments : Payment*\l- counts and capacities : int\l- fm : FileManager\l|+ FreelanceSystem()\l+ ~FreelanceSystem()\l+ createProject() : void\l+ assignFreelancer() : void\l+ makePayment() : void\l+ closeProject() : void\l+ cancelProject() : void\l+ report() : void\l+ saveData() : void\l+ loadData() : void\l+ run() : void\l- findClient(id) : int\l- paidFor(projectId) : double\l}"];
Utils [label="{«namespace»\nUtils|+ readLine / readInt / readDouble\l+ isValidName / isValidPhone\l+ isValidText / isValidDate\l+ toLower(s) : string\l+ grow\<T\>(arr, count, cap)\l}", style=dashed];
Client -> Person [arrowhead=empty]; Freelancer -> Person [arrowhead=empty];
FS -> Client [dir=back, arrowtail=diamond, label="0..*"]; FS -> Freelancer [dir=back, arrowtail=diamond, label="0..*"];
FS -> Project [dir=back, arrowtail=diamond, label="0..*"]; FS -> Payment [dir=back, arrowtail=diamond, label="0..*"];
FS -> FileManager [arrowhead=vee, label="uses"]; FS -> Utils [style=dashed, arrowhead=vee, label="uses"];
Project -> Client [style=dashed, arrowhead=vee, label="clientId"];
Project -> Freelancer [style=dashed, arrowhead=vee, label="freelancerId"];
Payment -> Project [style=dashed, arrowhead=vee, label="projectId"];
}'''
open("class.dot","w").write(cls)
subprocess.run(["dot","-Tpng","-Gdpi=130","class.dot","-o","Class_Diagram.png"],check=True)

# ---------- Use case (graphviz) ----------
uc = r'''digraph U { rankdir=LR; fontname="Helvetica"; node [fontname="Helvetica", fontsize=10];
Admin [shape=box, label="«actor»\nAdministrator", style=filled, fillcolor="#e8f0fe"];
Cli [shape=box, label="«actor»\nClient\n(via Administrator desk)", style=filled, fillcolor="#e8f0fe"];
Fre [shape=box, label="«actor»\nFreelancer\n(via Administrator desk)", style=filled, fillcolor="#e8f0fe"];
subgraph cluster_s { label="Freelance Project Management System"; style=rounded;
 node [shape=ellipse, style=filled, fillcolor="#fff7e0"];
 U1 [label="Add / Update / Delete\nClients and Freelancers"]; U2 [label="Create Project"]; U3 [label="Assign Freelancer"];
 U4 [label="Record Payment"]; U5 [label="Close Project"]; U6 [label="Cancel Project"];
 U7 [label="Search / Display Records"]; U8 [label="View Summary Report"]; U9 [label="Save / Load Data"]; U10 [label="Estimate Freelancer Fee"]; }
Admin -> U1; Admin -> U2; Admin -> U3; Admin -> U4; Admin -> U5; Admin -> U6; Admin -> U7; Admin -> U8; Admin -> U9; Admin -> U10;
Cli -> U2; Cli -> U4; Cli -> U5; Cli -> U8; Fre -> U3; Fre -> U10; Fre -> U7;
}'''
open("usecase.dot","w").write(uc)
subprocess.run(["dot","-Tpng","-Gdpi=130","usecase.dot","-o","Use_Case_Diagram.png"],check=True)

def seq(title, parts, msgs, fn, notes=None):
    n=len(parts); step=0.62; top=0.0
    H=len(msgs)*step+1.5
    fig,ax=plt.subplots(figsize=(1.95*n+0.6,H*0.95)); ax.set_xlim(-0.1,n+0.1); ax.set_ylim(-len(msgs)*step-0.7,1.0); ax.axis("off")
    ax.set_title(title,fontsize=12,fontweight="bold",pad=6)
    for i,p in enumerate(parts):
        x=i+0.5
        ax.text(x,0.7,p,ha="center",va="center",fontsize=9,fontweight="bold",bbox=dict(boxstyle="round,pad=0.35",fc="#e8f0fe",ec="#333"))
        ax.plot([x,x],[0.5,-len(msgs)*step-0.45],ls="--",color="#888",lw=0.8,zorder=0)
    # activation bars (callee lifelines while handling a call)
    for k,(a,b,t,ret) in enumerate(msgs):
        y=-0.25-k*step; xa,xb=a+0.5,b+0.5
        if not ret and a!=b:
            ax.add_patch(plt.Rectangle((xb-0.04,y-0.28),0.08,0.5,fc="#dfe9fb",ec="#555",lw=0.6,zorder=1))
        lab=f"{k+1}. {t}"
        if a==b:
            ax.annotate("",xy=(xa+0.04,y-0.22),xytext=(xa+0.04,y),arrowprops=dict(arrowstyle="->",connectionstyle="arc3,rad=-1.6",lw=1))
            ax.text(xa+0.16,y-0.1,lab,fontsize=7.8,va="center",bbox=dict(fc="white",ec="none",pad=0.5))
        else:
            ax.annotate("",xy=(xb,y),xytext=(xa,y),arrowprops=dict(arrowstyle="->" if not ret else "->",ls="--" if ret else "-",lw=1),zorder=2)
            ax.text((xa+xb)/2,y+0.09,lab,ha="center",fontsize=7.8,bbox=dict(fc="white",ec="none",pad=0.5))
    if notes:
        ax.text(0.0,-len(msgs)*step-0.62,notes,fontsize=7.5,style="italic",color="#444")
    plt.tight_layout(); plt.savefig(fn,dpi=140,bbox_inches="tight"); plt.close()

P=["Admin","main()","FreelanceSystem","Project","Payment","FileManager"]
seq("Sequence Diagram 1: Record a Payment and Close a Project (main transaction)",P,[
 (0,1,"choose 6 > Record payment",0),(1,2,"run() > transactionMenu() > makePayment()",0),
 (0,2,"enter project ID, amount, date",0),(2,2,"findProject(id); status must be ASSIGNED",0),
 (2,4,"new Payment(id, projectId, amount, date)",0),(4,2,"validated (else invalid_argument)",1),
 (2,2,"paidFor(projectId) + amount <= budget ?",0),(2,2,"store in payments[] (grow if full)",0),
 (2,0,"\"Payment recorded ... Paid X of Y\"",1),
 (0,2,"choose 6 > Close project",0),(2,2,"findProject(id); balance == 0 ?",0),
 (2,3,"updateStatus(CLOSED)",0),(3,2,"ASSIGNED -> CLOSED (else logic_error)",1),
 (2,0,"\"Project closed\" or \"Error: ... still unpaid\"",1),
 (0,2,"choose 9 > Exit",0),(2,5,"save(clients, freelancers, projects, payments)",0),(5,2,"Data/*.dat written",1)],
 "Sequence_Diagram_1.png")
seq("Sequence Diagram 2: Assign a Freelancer to a Project",["Admin","FreelanceSystem","Project"],[
 (0,1,"choose 6 > Assign freelancer",0),(0,1,"enter Project ID and Freelancer ID",0),
 (1,1,"findProject / findFreelancer",0),(1,0,"\"Error: ... not found\" (if index < 0)",1),
 (1,1,"activeProjectOf(freelancerId) == 0 ?",0),(1,0,"\"Error: Freelancer is already working on project n\"",1),
 (1,2,"assign(freelancerId)",0),(2,2,"status == OPEN ? set ASSIGNED : throw logic_error",0),
 (2,1,"return / exception",1),(1,0,"\"Freelancer assigned. Project n is now Assigned.\"",1)],
 "Sequence_Diagram_2.png")
