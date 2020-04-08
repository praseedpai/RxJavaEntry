#include <iostream>

#include <memory>
#include <list>

using namespace std;



//////////////////////////////////
// List of operators supported by 
// the evaluator
//
//
enum class OPERATOR{
        //--- supports +,-,*/
        ILLEGAL,PLUS,MINUS,MUL,DIV,UNARY_PLUS,UNARY_MINUS
};

////////////////////////////
// Kinds of Expr -> Operator or a Value?
//
enum class ExprKind{
      ILLEGAL_EXP,  OPERATOR , VALUE
};
//////////////////////////////////////////
//
//
struct EXPR_ITEM {

    ExprKind knd;
    double Value;
    OPERATOR op;
    EXPR_ITEM():op(OPERATOR::ILLEGAL),Value(0),knd(ExprKind::ILLEGAL_EXP){}
    bool SetOperator( OPERATOR op ){
            this->op = op;
            this->knd = ExprKind::OPERATOR;
            return true;
    }
    bool SetValue(double value) {
            this->knd = ExprKind::VALUE;
            this->Value = value;
            return true;
    }

    string toString() {
        string ret_val = "";
        if ( this -> knd == ExprKind::OPERATOR )
           ret_val = ret_val + "Operator ";
        else
           ret_val = ret_val + "Operand --> ";

        return ret_val;

   }

    
};

class Number;
class BinaryExpr;
class UnaryExpr;
class IExprVisitor;

class Expr {
 public:
      virtual double accept(IExprVisitor* expr_vis) = 0;
      virtual ~Expr() {}

};

////////////////////////////////
// The Visitor interface contains methods for 
// each of the concerte node 
//
//
struct IExprVisitor{

       virtual  double Visit(Number& num) = 0;
       virtual  double Visit(BinaryExpr& bin) = 0;
       virtual  double Visit(UnaryExpr& un)=0 ;
};

///////////////////////////////
// A class to represent IEEE 754 interface
//
//
class Number : public Expr {
       private:
            double NUM;
       public:
            double getNUM() { return NUM;}   
            void SetNUM(double num)   { NUM = num; }
            Number(double n) { this->NUM = n; }
            ~Number() { cout << "Reached Number Destructor" << endl; }
            double accept(IExprVisitor* expr_vis)
            {
               return expr_vis->Visit(*this);
            }
};

///////////////////////////////////////
// Model BinaryExpr
//
//
class BinaryExpr : public Expr
{
       private:
            Expr* left;
            Expr* right;
            OPERATOR OP;

        

       public:
       BinaryExpr(Expr* l,Expr* r , OPERATOR op )
       {
	  left = l; right = r; OP = op;
       }
       OPERATOR getOP() { return OP; }
       Expr& getLeft() { return *left; }
       Expr& getRight() { return *right; }
       ~BinaryExpr() { delete left; delete right;  cout << "Reached Destructor " << endl; left =0; right=0; }
       double accept(IExprVisitor * expr_vis)
       {
            return expr_vis->Visit(*this);
       }
};

class UnaryExpr : public Expr 
{
   private:
     
     Expr * right;
     OPERATOR op;

   public:
            UnaryExpr( Expr *operand , OPERATOR op )
            { 
               
                right = operand;  this-> op = op;

            }

            Expr& getRight( ) { return *right; }
            OPERATOR getOP() { return op; }
            virtual ~UnaryExpr() { delete right; right = 0; cout << "Reached Destructor of Unary" << endl; }
            double accept(IExprVisitor* expr_vis)
            {
               return expr_vis->Visit(*this);
            }

};
//////////////////////////////////////////////////
// A Visitor to Evaluate Expression
//
//
class ReversePolishEvaluator : public IExprVisitor {

        public:
        double Visit(Number& num)
        {
            cout << num.getNUM() << " " << endl; 
            return 0;
        }
        double Visit(BinaryExpr& bin)
        {
            bin.getLeft().accept(this);
            bin.getRight().accept(this);
	    OPERATOR temp = bin.getOP();
            if (temp == OPERATOR::PLUS)
		cout << " + ";
            else if (temp == OPERATOR::MUL)
                cout << " * ";
            else if (temp == OPERATOR::DIV)
                cout << " / ";
            else if (temp == OPERATOR::MINUS)
                cout << " - " << endl;
           
         
            return 42;

        }
        double Visit(UnaryExpr& un)
        {
            OPERATOR temp = un.getOP();
            un.getRight().accept(this);
            if (temp == OPERATOR::UNARY_PLUS)
                cout << " ( + ) " << endl;
            else if (temp == OPERATOR::UNARY_MINUS)
                cout << " ( - ) "  << endl;
            
            
            return -1;
        }

    };


////////////////////////////////
//
// A Tree Walker Evaluator
//
class TreeEvaluatorVisitor : public IExprVisitor{
        
        public:
        double Visit(Number& num)
        {
            return num.getNUM();
        }

        double Visit(BinaryExpr& bin)
        {
           OPERATOR temp = bin.getOP();
           if (temp == OPERATOR::PLUS)
                return bin.getLeft().accept(this) + bin.getRight().accept(this);
            else if (temp == OPERATOR::MUL)
            return bin.getLeft().accept(this) * bin.getRight().accept(this);
            else if (temp == OPERATOR::DIV)
            return bin.getLeft().accept(this) / bin.getRight().accept(this); 
            else if (temp == OPERATOR::MINUS)
              return bin.getLeft().accept(this) - bin.getRight().accept(this);

             return -1;

        }

        double Visit(UnaryExpr& un)
            
        {
           OPERATOR temp = un.getOP();
           if (temp == OPERATOR::UNARY_PLUS)
                return +  un.getRight().accept(this);
           else  if (temp == OPERATOR::UNARY_MINUS)
                return - un.getRight().accept(this);
             return -1;

        }
};


/////////////////////////////////////
// A Flattener for Expressions
//
class FlattenVisitor : public IExprVisitor {
        
       private: 
        list<EXPR_ITEM>  ils;

        EXPR_ITEM MakeListItem(double num)
        {
            EXPR_ITEM temp;
            temp.SetValue(num);
            return temp;
        }

        EXPR_ITEM MakeListItem(OPERATOR op)
        {
            EXPR_ITEM temp;
            temp.SetOperator(op);

            return temp;
        }

        public:

        list<EXPR_ITEM> FlattenedExpr()
        {
            return ils;
        }
        FlattenVisitor()
        {
            
        }
         double Visit(Number& num)
        {
        
            ils.push_back(MakeListItem(num.getNUM()));
            return 0;
        }
        double Visit(BinaryExpr& bin)
        {
            bin.getLeft().accept(this);
            bin.getRight().accept(this);

            ils.push_back(MakeListItem(bin.getOP()));
        }
        double Visit(UnaryExpr& un)
        {

            un.getRight().accept(this);
            ils.push_back(MakeListItem(un.getOP()));
            return 0;
          

        }

    };

void Iterate( list<EXPR_ITEM>& s )
{

     // Iterate and print values of the list
    for (auto n : s ) {
        std::cout << n.toString()  << '\n';
    }

}


int main()
{
     unique_ptr<Expr>  a (new BinaryExpr( new Number(10) , new Number(20) , OPERATOR::PLUS));

     unique_ptr<IExprVisitor>  exp(new ReversePolishEvaluator());
     
     a->accept(&(*exp));
     cout << endl <<  "........................" << endl;

     unique_ptr<IExprVisitor> eval( new TreeEvaluatorVisitor());

     double result = a->accept( &(*eval));

     cout << result << endl;

     unique_ptr<FlattenVisitor> flt( new FlattenVisitor() );

     a->accept( & (*flt));

     list<EXPR_ITEM> n = flt->FlattenedExpr();

     Iterate(n);
}
