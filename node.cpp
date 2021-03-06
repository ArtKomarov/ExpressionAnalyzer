#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "node.h"
#include "analyzerconsts.h"

// Used to make graph by graphwiz
const char GraphHeader[] = "digraph G{\n "
                                  "node [style=\"filled\", fillcolor=\"orange\", fontcolor=\"blue\"];\n";

// Node::Constructor == Node::Node
//Free construtor
Node::Node() {}

// Construtor for numerical nodes
Node::Node(int type, nod_val val) {
    switch(type) {
    case NUM: {
        this->type = type;
        this->val  = val;
        break;
    }
    case OP: {
        fprintf(stderr, "Node::Construct for numbers used for operation!\n");
        NodeSet(type, val, new Node(NUM, 0), new Node(NUM, 0));
        break;
    }
    case VAR: {
        fprintf(stderr, "Node::Construct for numbers used for variable!\n");
        NodeSet(type);
        break;
    }
    default:
        this->type = type;
        this->val  = val;
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    }
}

// Construtor for variable nodes
Node::Node(int type) {
    switch(type) {
    case VAR:
        this->val  = 'x';
        this->type = VAR;
        break;
    case NUM:
        fprintf(stderr, "Node::Construct for variables used for number!\n");
        NodeSet(type, 0);
        break;
    case OP:
        fprintf(stderr, "Node::Construct for variables used for operation!\n");
        NodeSet(type, 0, new Node(NUM, 0), new Node(NUM, 0));
        break;
    default:
        this->type = type;
        this->val = 'U';
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    }
}

// Construtor for operation nodes, but may use for any else nodes
Node::Node(int type, nod_val val, Node* left, Node* right) {
    switch(type) {
    case OP: {
        this->type  = type;
        this->val   = val;
        if(left == NULL) {
            fprintf(stderr, "Node::Construct: left child of operation node is NULL!\n");
            this->left = new Node(NUM, 0);
        }
        else
            this->left  = left;
        this->right = right;
        break;
    }
    case VAR: {
        if(left != NULL || right != NULL)
            fprintf(stderr, "Node::Construct for operation used for variable!\n");
        NodeSet(type);
        break;
    }
    case NUM: {
        if(left != NULL || right != NULL)
            fprintf(stderr, "Node::Construct for operation used for number!\n");
        NodeSet(type, val);
        break;
    }
    default: {
        this->type  = type;
        this->val   = val;
        this->left  = left;
        this->right = right;
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    }
    }
}

// Construtor for unary operation nodes
Node::Node(int type, nod_val val, Node* left) {
    assert(left  != NULL);
    switch(type) {
    case OP: {
        this->type  = type;
        this->val   = val;
        this->left  = left;
        break;
    }
    case NUM: {
        fprintf(stderr, "Node::Construct for operations used for number!\n");
        NodeSet(type, val);
        break;
    }
    case VAR: {
        fprintf(stderr, "Node::Construct for operations used for variable!\n");
        NodeSet(type);
        break;
    }
    default:
        this->type  = type;
        this->val   = val;
        this->left  = left;
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    }
}

// Recursive destructor
Node::~Node() {
    if(this->left != NULL) {
        delete this->left;
        this->left = NULL;
    }
    if(this->right != NULL) {
        delete this->right;
        this->right = NULL;
    }
}

// Node fileds setter for numerical nodes
void Node::NodeSet(int type, nod_val val) {
    if(type != OP && type != VAR && type != NUM)
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    this->type  = type;
    this->val   = val;
}

// Node fileds setter for variable nodes
void Node::NodeSet(int type) {
    if(type != OP && type != VAR && type != NUM)
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    this->type  = type;
    this->val   = 'x';
}

// Node fileds setter for operation nodes
void Node::NodeSet(int type, nod_val val, Node* left, Node* right) {
    if(type != OP && type != VAR && type != NUM)
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    this->type  = type;
    this->val   = val;
    this->left  = left;
    this->right = right;
}

// Node fileds setter for unary operation nodes
void Node::NodeSet(int type, nod_val val, Node* left) {
    if(type != OP && type != VAR && type != NUM)
        fprintf(stderr, "Node::Construct: unknown type! Number of type %d, equal character %c.\n", type, type);
    this->type  = type;
    this->val   = val;
    this->left  = left;
}

// Recursive copy tree, whose root is this node
Node* Node::NodeCopy() {
    Node* left = NULL;
    Node* right = NULL;
    if(this->left != NULL)
        left = this->left->NodeCopy();
    if(this->right != NULL)
        right = this->right->NodeCopy();
    return new Node(this->type, this->val, left, right);
}

// Value getter
nod_val Node::GetVal() {
    return this->val;
}

// Support function for Node::MakeGraphFile - prints node
int Node::FPrintGraphNode(FILE* fd) {
    long id      = this - (Node*)0x0; // Unique id for every node, that helps building correct the graph
    long leftid  = 0;
    long rightid = 0;
    if(this->left != NULL) {
        leftid = this->left - (Node*)0x0;
        fprintf(fd, "%ld [label = ", leftid);
        if(this->left->type == NUM) {
            fprintf(fd, "\"%d\"]\n", this->left->val);
        }
        else
            fprintf(fd, "\"%c\"]\n", this->left->val);
        fprintf(fd, "%ld -> %ld\n", id, leftid);
    }
    if(this->right != NULL) {
        rightid = this->right - (Node*)0x0;
        fprintf(fd, "%ld [label = ", rightid);
        if(this->right->type == NUM) {
            fprintf(fd, "\"%d\"]\n", this->right->val);
        }
        else
            fprintf(fd, "\"%c\"]\n", this->right->val);
        fprintf(fd, "%ld -> %ld\n", id, rightid);
    }
    if(this->left != NULL)
        this->left->Node::FPrintGraphNode(fd);
    if(this->right != NULL)
        this->right->Node::FPrintGraphNode(fd);
    return 0;
}

//Making file, that used by graphwiz to build graph
int Node::MakeGraphFile(char* FileName) {
    assert(FileName != NULL);
    FILE* fd;
    if((fd = fopen(FileName, "w")) == NULL) {
        perror("fopen");
        return -1;
    }
    fputs(GraphHeader, fd);
    long id = this - (Node*)0x0;
    fprintf(fd, "%ld [label = ", id);
    if(this->type == NUM) {
        fprintf(fd, "\"%d\"]\n", this->val);
    }
    else
        fprintf(fd, "\"%c\"]\n", this->val);
    this->Node::FPrintGraphNode(fd);
    fprintf(fd, "}\n");
    fclose(fd);
    return 0;
}

// Support function for Node::SaveTree, prints node
int Node::FPrintNode(FILE* fd) {
    assert(fd != NULL);
    printf("%d %d\n", this->type, this->val);
    fprintf(fd, "%d %d\n", this->type, this->val);
    if(this->left != NULL) {
        printf("(");
        fprintf(fd, "(");
        this->left->Node::FPrintNode(fd);
    }
    if(this->right != NULL) {
        printf(",");
        fprintf(fd, ",");
        this->right->Node::FPrintNode(fd);
        printf(")\n");
        fprintf(fd, ")\n");
    }
    else if(this->left != NULL) {
        printf(")\n");
        fprintf(fd, ")\n");
    }
    return 0;
}

// Simple saving graph (expression) to file for future reset
int Node::SaveTree(char* filename) {
    assert(filename != NULL);
    FILE* fd;
    if((fd = fopen(filename, "w")) == NULL) {
        perror("fopen");
        return -1;
    }
    printf("%d %d\n", this->type, this->val);
    fprintf(fd, "%d %d\n", this->type, this->val);
    if(this->left != NULL)
        this->left->Node::FPrintNode(fd);
    if(this->right != NULL)
        this->right->Node::FPrintNode(fd);
    fclose(fd);
    return 0;
}

// Build tree from file
void Node::TreeConstruct(char* filename) {
    assert(filename != NULL);
    FILE* fd;
    if((fd = fopen(filename, "r")) == NULL) {
        perror("fopen");
        return;
    }
    this->left  = NULL;
    this->right = NULL;
    if(fscanf(fd, "%d%d", &this->type, &this->val) == EOF) {
        fclose(fd);
        return;
    }
    char buff[MAX_NAME_LEN+1];
    fgets(buff, MAX_NAME_LEN, fd);
    this->FScanNode(buff, fd);
    fclose(fd);
}

//Support function for Node::TreeConstruct (scan node)
int Node::FScanNode(char* CurStr, FILE* fd) {
    assert(fd  != NULL);
    //char buff[MAX_NAME_LEN+1];
    if(fgets(CurStr, MAX_NAME_LEN + 1, fd) == NULL || CurStr[0] == '\n') {
        return -1;
    }
    else {
        int BuffLen = strlen(CurStr);
        if(BuffLen > 0 && CurStr[BuffLen - 1] == '\n')
            CurStr[BuffLen - 1] = '\0';
        if(CurStr[0] != '(')
            return 0;
        else {
            this->left = new Node;
            sscanf(CurStr + 1, "%d%d", &this->left->type, &this->left->val);
            if(this->left->Node::FScanNode(CurStr, fd) == -1) return -1;
            if(CurStr[0] != ',') {
                if(CurStr[0] != ')') {
                    fprintf(stderr, "Invalid format of input file!!!\n");
                    return -1;
                }
            }
            else {
                this->right = new Node;
                sscanf(CurStr + 1, "%d%d", &this->right->type, &this->right->val);
                if(this->right->Node::FScanNode(CurStr, fd) == -1) return -1;
                if(CurStr[0] != ')') {
                    fprintf(stderr, "Invalid format of input file!!!\n");
                    return -1;
                }
            }
            fgets(CurStr, MAX_NAME_LEN + 1, fd);
        }
    }
    return 0;
}

// Calculate number of nodes in tree, started by this root (used by destructor)
int Node::NumberOfNodes() {
    int i = 0;
    if(this->left != NULL)
        i += this->left->NumberOfNodes();
    if(this->right != NULL)
        i += this->right->NumberOfNodes();
    return i + 1;
}

/*
// Calculate expression, that contain in this tree
nod_val Node::TreeCount(nod_val var) {
    switch(this->type) {
    case OP:
        if(this->left == NULL) {
            fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
            return 0;
        }
        switch(this->val) {
        case '+':
            if(this->right == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
            }
            return this->left->TreeCount(var) + this->right->TreeCount(var);
        case '-':
            if(this->right == NULL) {
                //fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
                return (nod_val)((-1) * this->left->TreeCount(var));
            }
            return this->left->TreeCount(var) - this->right->TreeCount(var);
        case '*':
            if(this->right == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
            }
            return this->left->TreeCount(var) * this->right->TreeCount(var);
        case '/':
            if(this->right == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
            }
            return this->left->TreeCount(var) / this->right->TreeCount(var);
        case '^':
            if(this->right == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
            }
            return (nod_val) pow(this->left->TreeCount(var), this->right->TreeCount(var));
        case 's':
            return (nod_val) sin(this->left->TreeCount(var));
        case 'c':
            return (nod_val) cos(this->left->TreeCount(var));
        case 'l':
            return (nod_val) log(this->left->TreeCount(var));
        default:
            fprintf(stderr, "There is broken tree, unknown operation \"%c\"!\n", this->val);
            return 0;
        }
        break;
    case NUM:
        return this->val;
    case VAR:
        if(var != NAN)
            return var;
        else {
            fputs("TreeCount: Variable value is NAN!\n", stderr);
            return 0;
        }
    default:
        fprintf(stderr, "There is broken tree, unknown type number \"%d\"!\n", this->type);
        return 0;
    }
}

// Operations with nodes
Node* Node::operator +(Node* other) {
    return new Node(OP, '+', this, other);
}

Node* Node::operator -(Node* other) {
    return new Node(OP, '-', this, other);
}

Node* Node::operator -(int other) {
    return new Node(OP, '-', this, new Node(NUM, other));
}

Node* Node::operator *(Node* other) {
    return new Node(OP, '*', this, other);
}

Node* Node::operator /(Node* other) {
    return new Node(OP, '/', this, other);
}

Node* Node::operator ^(Node* other) {
    return new Node(OP, '^', this, other);
}

Node* Node::operator ^(int other) {
    return new Node(OP, '^', this, new Node(NUM, other));
}

// Reccursive check, does this tree contain variable node
bool Node::IsVar() {
    if(this->type == VAR)
        return true;
    bool VarLeft, VarRight;
    if(this->left != NULL)
        VarLeft = this->left->IsVar();
    else
        VarLeft = false;
    if(this->right != NULL)
        VarRight = this->right->IsVar();
    else
        VarRight = false;
    return VarLeft || VarRight;
}

// Differentiation this tree
Node* Node::Diff() {
    if(this->type == NUM)
        return new Node(NUM, 0);
    if(this->type == VAR)
        return new Node(NUM, 1);
    if(this->type != OP) {
        fprintf(stderr, "Unknown type of node!\n");
        return NULL;
    }

    switch (this->val) {
    case '+':
        return *this->left->Diff() + this->right->Diff();
        break;
    case '-':
        if(this->right == NULL) {
            return new Node(OP, '-', this->left->Diff());
        }
        return *this->left->Diff() - this->right->Diff();
        break;
    case '*':
        return *(*this->left->Diff() * this->right->NodeCopy()) + (*this->left->NodeCopy() * this->right->Diff());
        break;
    case '/':
        return *(*(*this->left->Diff() * this->right->NodeCopy()) - (*this->left->NodeCopy() * this->right->Diff())) / (*this->right->NodeCopy() ^ 2);
        break;
    case 's':
        if(this->left == NULL) {
            fprintf(stderr, "Diff: No argument in sin!\n"); //argument of sin/cos must be left child
            return this->NodeCopy();
        }
        return *new Node(OP, 'c', this->left->NodeCopy()) * this->left->Diff();
        break;
    case 'c':
        if(this->left == NULL) {
            fprintf(stderr, "Diff: No argument in cos!\n"); //argument of cos/sin must be left child
            return this->NodeCopy();
        }
        return *(*new Node(NUM, 0) - new Node(OP, 's', this->left->NodeCopy())) * this->left->Diff();
        break;
    case '^':
        if(this->left == NULL || this->right == NULL) {
            fprintf(stderr, "Diff: Left or right child does not exists for /'^/'!\n");
            return this->NodeCopy();
        }

        //May be chack IsVar, but it will do optimization
        if(this->right->IsVar() == false) { // (x^a)' = a * x^(a-1) * x'
            Node* mul1 = this->right->NodeCopy();
            Node* mul2 = *this->left->NodeCopy() ^ (*this->right->NodeCopy() - 1);
            Node* mul3 = this->left->Diff();
            return *mul1 * (*mul2 * mul3);
        }
        else if(this->left->IsVar() == false){ // (a^x)' = a^x * ln(a) * x'
            Node* mul1 = this->NodeCopy();
            Node* mul2 = new Node(OP, 'l', this->left->NodeCopy());
            Node* mul3 = this->right->Diff(); //?
            return *mul1 * (*mul2 * mul3);
        }
        else { // (f^g)' = (e ^ (g * ln(f)))' = (f^g) * (g' * ln(f) + (f' / f) * g) = (x^x)(ln(x) + 1/x * x)
            Node* mul1 = this->NodeCopy();
            Node* add1 = *this->right->Diff() * (new Node(OP, 'l', this->left->NodeCopy()));
            Node* add2 = *(*this->left->Diff() / this->left->NodeCopy()) * this->right->NodeCopy();
            return *mul1 * (*add1 + add2);
        }
        break;
    case 'l': // ln(x)' = (1/x) * x'
        if(this->left == NULL) {
            fprintf(stderr, "Diff: Left child does not exists for /'ln/'!\n");
            return this->NodeCopy();
        }
        return new Node(OP, '/', this->left->Diff(), this->NodeCopy());
        break;
    default:
        fprintf(stderr, "Diff: Unknown operation \"%c\"!\n", this->val);
        return NULL;
        break;
    }
}*/

// Reccursive optimization, delete nodes, that don't effect on the value of expression
int Node::Optimization() {
    switch(this->type) {
    case NUM:
        return this->val;
    case VAR:
        return -1;
    case OP: {
        int LeftVal  = 0;
        int RightVal = 0;
        if(this->left != NULL)
            LeftVal  = this->left->Optimization();
        if(this->right != NULL)
            RightVal = this->right->Optimization();
        Node* Left  = this->left;
        Node* Right = this->right;
        switch (this->val) {
        case '+':
        case '-':
            if(RightVal == 0 && LeftVal == 0) {
                delete this->left;
                delete this->right;
                this->left  = NULL;
                this->right = NULL;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if(RightVal == 0) {
                delete this->right;
                this->right = NULL;
                this->NodeSet(this->left->type, this->left->val, this->left->left, this->left->right);
                Left->left  = NULL;
                Left->right = NULL;
                delete Left;
                Left = NULL;
                return this->val;
            }
            else if(LeftVal == 0) {
                delete this->left;
                this->left = NULL;
                if(this->val == '-')
                    return '-';
                this->NodeSet(this->right->type, this->right->val, this->right->left, this->right->right);
                Right->left  = NULL;
                Right->right = NULL;
                delete Right;
                Right = NULL;
                return this->val;
            }
            break;

        case '*':
            if(LeftVal == 0 || RightVal == 0) {
                delete this->left;
                delete this->right;
                this->left  = NULL;
                this->right = NULL;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if (RightVal == 1 && LeftVal == 1) {
                delete this->left;
                delete this->right;
                this->left  = NULL;
                this->right = NULL;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete this->right;
                this->right = NULL;
                this->NodeSet(this->left->type, this->left->val, this->left->left, this->left->right);
                Left->left  = NULL;
                Left->right = NULL;
                delete Left;
                Left = NULL;
                return this->val;
            }
            else if(LeftVal == 1) {
                delete this->left;
                this->left = NULL;
                this->NodeSet(this->right->type, this->right->val, this->right->left, this->right->right);
                Right->left  = NULL;
                Right->right = NULL;
                delete Right;
                Right = NULL;
                return this->val;
            }
            break;
        case '/':
            if(RightVal == 0) {
                fprintf(stderr, "В дереве есть деление на ноль!\n");
                return '/';
            }
            else if(LeftVal == 0) {
                delete this->left;
                delete this->right;
                this->left  = NULL;
                this->right = NULL;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if (RightVal == 1 && LeftVal == 1) {
                delete this->left;
                delete this->right;
                this->left  = NULL;
                this->right = NULL;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete this->right;
                this->right = NULL;
                this->NodeSet(this->left->type, this->left->val, this->left->left, this->left->right);
                Left->left  = NULL;
                Left->right = NULL;
                delete Left;
                Left = NULL;
                return this->val;
            }
            break;
        case 's':
        case 'c':
            if(LeftVal == 0) {
                delete this->left;
                this->left  = NULL;
                if(this->val == 's')
                    this->NodeSet(NUM, 0);
                else
                    this->NodeSet(NUM, 1);
                return this->val;
            }
            break;
        case '^':
            if(LeftVal == 0) {
                if(RightVal < 0) {
                    fprintf(stderr, "В дереве есть деление на 0!\n");
                    return '^';
                }
                else {
                    delete this->left;
                    delete this->right;
                    this->left  = NULL;
                    this->right = NULL;
                    this->NodeSet(NUM, 0);
                    return 0;
                }
            }
            if(LeftVal == 1 || RightVal == 0) {
                delete this->left;
                delete this->right;
                this->left  = NULL;
                this->right = NULL;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete this->right;
                this->right = NULL;
                this->NodeSet(this->left->type, this->left->val, this->left->left, this->left->right);
                Left->left  = NULL;
                Left->right = NULL;
                delete Left;
                Left = NULL;
                return this->val;
            }
            break;
        case 'l':
            return this->val;
        default:
            fprintf(stderr, "Unknown operation %c!\n", this->val);
            break;
        }
        break;
    }
    default:
        fprintf(stderr, "Optimization: Unknown type \"%d\"!\n", this->type);
    }
    return this->val;
}

