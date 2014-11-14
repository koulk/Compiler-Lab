#include<stdio.h>
#include<stack>
#include<set>
#include<string.h>
#include<stdlib.h>
#include<iostream>

using namespace std;

#define MAX 200

int state_table[MAX][28],final_state,dfa_state,initial_state;
int dfa_state_table[2*MAX][26];
char postfix[100];
int arr_final[2*MAX];
int state_no=0;

std::stack<char> op_stack;
std::stack<int> state_stack;
std::set<int> individual;
std::set<int>* subset;
  
void covert_Postfix(char *);
void NFA(char *);
void kleene_closure();
void literal(int);
void or_operation(); 
void and_operation();
void plus_operation();
void display();
void DFA();
void e_closure();
int e1_closure(int);
int e2_closure(int);
void display_DFA(); 
int check_exist(int);


int  main()
{

char regular_expression[100];
int i,j;

printf("Input SYMBOLS a-z\n\n");
 printf("Initial Regex\n");
 scanf("%s",regular_expression);
 //printf("%s",regular_expression);
 
        covert_Postfix(regular_expression); //convert into postfix
        printf("%s",postfix);

         
          //Initialise
          for(i=0;i<MAX;i++)
         {
            for(j=0;j<28;j++)
            {
               state_table[i][j] =-1;
            }
         }



          //NFA(regular_expression);
          NFA(postfix); //convert postfix into nfa
          display();
          DFA();
          display_DFA();
       
return 0;
}


void covert_Postfix(char* regex)
{
    int i,counter=-1,len;
    
      len=strlen(regex); // length of original regex
      
    op_stack.push('(');       
    regex[len]=')';
     
    for (i = 0; i<=len; ++i)
    {
       // printf("%c",regex[i]);
                 
        if ((regex[i]>= 'a' && regex[i] <= 'z') || regex[i]=='*' || regex[i]=='+') //operand and uninary op *,+ pushed straight into postfix
            postfix[++counter] = regex[i];
         
        
        else if (regex[i] == '(')  // if ( push to operator stack
            op_stack.push(regex[i]);
        

        else if (regex[i] == ')')
        {
            while (!(op_stack.empty()) && (op_stack.top()!= '('))  //  till stack empty or ) encountered pop stack elements into postfix
                {
                  postfix[++counter] = op_stack.top();
                  op_stack.pop();
                }
                
                
                if(op_stack.empty()||op_stack.top()!='(')
                {
                       printf("\nInvalid Expression\n");   // more ) than (
                       exit(0);
                    
                }
                
                else
                  op_stack.pop();
        }
        

        else
        {
        
                 if(op_stack.top()=='(')
                    {
                       op_stack.push(regex[i]);
                       continue;
                    }
                    
            while (!(op_stack.empty()) && op_stack.top()!='(' && (regex[i]>=op_stack.top()) ) // . Ascii 46 has greater precedence than | Ascii 124   
                {
                   postfix[++counter] = op_stack.top();
                   op_stack.pop();
                }
               
                
               op_stack.push(regex[i]);
        }
 
    }
 
  
          while (!(op_stack.empty()))               // pop all remaining into postfix
                {
                  postfix[++counter] = op_stack.top();
                  op_stack.pop();
                }
 
    postfix[++counter] = '\0';
    
}


void NFA(char *postfix)
{
  int i;
  

     for(i=0; postfix[i]!='\0'; i++)
      {
          if(postfix[i]>='a' && postfix[i]<='z')
             literal(postfix[i]-'a');
             
          else if(postfix[i]=='*')
             kleene_closure();
            
          else if(postfix[i]=='|')
             or_operation();
            
          else if(postfix[i]=='.')
             and_operation();
        
          else if(postfix[i]=='+')
             plus_operation();
                  
          else
            {
               printf("\nInvalid Operation\n");
               exit(0);
            }
          
      }
   
    if(state_stack.size()>2)
    {
            printf("\nInvalid Expression\n");
            exit(0);      
    }
  
      
// final 2 elements in stack should be start and end state
      
final_state = state_stack.top();
state_stack.pop();

initial_state = state_stack.top();
state_stack.pop();

   printf("\n\nInitial State %d\nFinal State %d\n",initial_state,final_state);
   
}


void DFA()
{
  int current_state,i,j,check,temp,dfa_initial_state,final_no=0;
    
  subset = new set<int>[200];
  current_state=0;
  dfa_state=0;
  
  //Initialise State Transition Table
    for(i=0;i<2*MAX;i++)
     for(j=0;j<26;j++)
       dfa_state_table[i][j]=-1;
   
    
   individual.insert(initial_state); // initial state of nfa
      
           
   // e-closure of initial set
             e_closure();
             
             //update initial set

             for (std::set<int>::iterator it=individual.begin(); it!=individual.end(); ++it)
              {
                  subset[current_state].insert(*it);
                 
              }
    

    
    while(current_state<=dfa_state)
    {     
                       
              individual.clear(); // clear set for future use
             
              // input transitions for nfa state in a set
              
              for(i=0;i<26;i++)
              {
                  individual.clear();
                  
                   for (std::set<int>::iterator it=subset[current_state].begin(); it!=subset[current_state].end(); ++it)
                  {
                       temp=state_table[*it][i];
                       
                         if(temp!=-1)    // if transition exists
                         {
                             individual.insert(temp);
                         }
                                
                                   	  
                  }
                  
                  
                     if(individual.size()>0)  // if any valid transition for input i
                      {
                         e_closure();
                         check=1;
                         
                             //check if state exists before 
                                 for(j=0;j<=dfa_state;j++)
                                 {
                                     if(subset[j]==individual)
                                     {
                                          dfa_state_table[current_state][i]=j; // transition for same set 
                                          check=0;
                                          //cout<<"state before"<<dfa_state<<endl;
                                          break;
                                     }
                                 
                                 }   
                               
                               // if state doesnt exist before         
                                if(check)
                                {

                                    ++dfa_state;
                                    
                                  for (std::set<int>::iterator it=individual.begin(); it!=individual.end(); ++it)
              			    {
                  			subset[dfa_state].insert(*it);
                  			
              		            }
              		            

              		            dfa_state_table[current_state][i]=dfa_state; // transition for new set
              		         }                        
                          
                      }
                        
                      
                  
                
              }
              
      
           current_state++;
    }
   
   
      // Finding Intitial and Final States in DFA
        for(i=0;i<=dfa_state;i++)
        {
              for (std::set<int>::iterator it=subset[i].begin(); it!=subset[i].end(); ++it)
                  {
                        if(*it==final_state)
                        {
                             arr_final[final_no++]=i;
                        }
                        
                        if(*it==initial_state)
                        {
                           dfa_initial_state=i;
                        }
                  }
        }
                 
                 
                 
                 cout<<"DFA Intital State "<<dfa_initial_state<<endl;
                 cout<<"DFA Final State ";
                 for(i=0;i<final_no;i++)
                 cout<<arr_final[i]<<" ";
                 
                 cout<<endl; 
                  
}


void e_closure()
{
 int prev_size,current_size;


    prev_size=current_size=individual.size();
      
      do
         {
             prev_size=current_size;
             
              for (std::set<int>::iterator it=individual.begin(); it!=individual.end(); ++it)
              {
                       individual.insert(e1_closure(*it));
                       individual.insert(e2_closure(*it));
              }

              current_size=individual.size();
         }
         
          while(prev_size!=current_size);
          

}

int e1_closure(int num)
{
  int temp=state_table[num][26];
      if(temp!=-1)
          return temp;
      else
         return num; // same num wont get inserted into set due to set property     

}

int e2_closure(int num)
{
  int temp=state_table[num][27];
  
      if(temp!=-1)
          return temp;
      else
         return num; // same num wont get inserted into set due to set property     

}


int check_exist(int index)
{

 std::set<int>::iterator it=individual.begin();
 std::set<int>::iterator it1=subset[index].begin();

      if(individual.size()!=subset[index].size())
        return 0;
        
         for (; it!=individual.end(); ++it)
              {
                   if(*it!=*it1)
                    return 0;
                    
                 ++it1;
              }
 return 1;
}


void literal(int input)
{
 
 state_table[state_no][input] = state_no+1;  // on state_no for input goes to state_no+1
  
  //Push 1 & 2 states of literal onto stack
  state_stack.push(state_no);   
  state_stack.push(state_no+1);

 state_no += 2;
}


void and_operation()
{
int beg1, beg2, end1, end2;


   if(state_stack.size()<4)
    {
            printf("\nInvalid Expression\n");
            exit(0);      
    }

   //extracting beginning and end of the block
   
	end2 = state_stack.top();
	state_stack.pop();

	beg2 = state_stack.top();
	state_stack.pop();

	end1 = state_stack.top();
	state_stack.pop();

	beg1=state_stack.top();
	state_stack.pop();

//Transitions
	state_table[end1][26] = beg2;  // epsilon1 from a to b
	state_table[state_no][26]=beg1;  // epsilon1 from new state to a
	state_table[end2][26]=state_no+1; // epsilon1 from b to next new state

// new start and end of block pushed

state_stack.push(state_no);
state_stack.push(state_no+1);
state_no+=2;
}


void or_operation()
{

int beg1, beg2, end1, end2;

   
   if(state_stack.size()<4)
    {
            printf("\nInvalid Expression\n");
            exit(0);      
    }

   //extracting beginning and end of the block
   
	end2 = state_stack.top();
	state_stack.pop();

	beg2 = state_stack.top();
	state_stack.pop();

	end1 = state_stack.top();
	state_stack.pop();

	beg1=state_stack.top();
	state_stack.pop();



//Transitions
	state_table[end1][26] = state_no+1; // epsilon1 from b to next state
	state_table[end2][26] = state_no+1; // epsilon1 from d to next state
	state_table[state_no][26] = beg1;   //  epsilon1 from prev state to a
	state_table[state_no][27] = beg2;   // epsilon1 from prev state to c

// new start and end of block pushed

state_stack.push(state_no);
state_stack.push(state_no+1);
state_no += 2;

}

void kleene_closure()
{
int beg,end;

 
   if(state_stack.size()<2)
    {
            printf("\nInvalid Expression\n");
            exit(0);      
    }

//extracting beginning and end of the block
   
        end = state_stack.top();
	state_stack.pop();

	beg = state_stack.top();
	state_stack.pop();

//Transitions
	state_table[end][27] = beg;  //epsilon 2 from b to a
	state_table[end][26] = state_no+1; //epsilon 1 from b to next state
	state_table[state_no][26] = beg;  //epsilon 1 from prev state to a
	state_table[state_no][27] = state_no+1; //epsilon 2 from prev state to next state

// new start and end of block pushed
state_stack.push(state_no);
state_stack.push(state_no+1);

state_no += 2;
}


void plus_operation()
{
int beg,end;

 
   if(state_stack.size()<2)
    {
            printf("\nInvalid Expression\n");
            exit(0);      
    }

//extracting beginning and end of the block
   
        end = state_stack.top();
	state_stack.pop();

	beg = state_stack.top();
	state_stack.pop();

//Transitions
	state_table[end][27] = beg;  //epsilon 2 from b to a
	state_table[end][26] = state_no+1; //epsilon 1 from b to next state
	state_table[state_no][26] = beg;  //epsilon 1 from prev state to a
	

// new start and end of block pushed
state_stack.push(state_no);
state_stack.push(state_no+1);

state_no += 2;
}

void display()
{

int i,j;

printf("STATE TRANSITION TABLE\n\n");

   printf("State\\""\tInputs");
   
    printf("  ");
     for(i=97;i<=122;i++)
       printf("%c ",i);
       
     printf("E1 E2\n");

      for(i=0;i<state_no;i++)
      {
      
        printf("%d\t\t",i);
        
         for(j=0;j<28;j++)
         {
           if(state_table[i][j]==-1)
            {
              printf("- ");
              continue;
            }
            
           printf("%d ",state_table[i][j]);
         }
         
         printf("\n");
      }
             
}

void display_DFA()
{
int i,j;

printf("\n\nDFA TRANSITION TABLE\n\n");

   printf("State\\""\tInputs");
   
    printf("  ");
     for(i=97;i<=122;i++)
       printf("%c ",i);
       
       printf("\n");
  
      for(i=0;i<=dfa_state;i++)
      {
      
        printf("%d\t\t",i);
        
         for(j=0;j<26;j++)
         {
           if(dfa_state_table[i][j]==-1)
            {
              printf("- ");
              continue;
            }
            
           printf("%d ",dfa_state_table[i][j]);
         }
         
         printf("\n");
      }
    

}

