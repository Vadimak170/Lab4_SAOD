#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <clocale>
using namespace std;
const int password=4352;
char* filename = "dbase.txt";
enum Action { INSERT, DEL, INFO };
enum Dir { LEFT, RIGHT };
const int l_time = 20, l_type = 40, l_number = 12, l_marka=20;
struct Fine {//---------------����� (������� ������)
char time[l_time];//����� ���������
char type[l_type];//��� ���������
char marka[l_marka];
float price;//������ ������
Fine* next;//��������� �� ��������� �������
};

struct Node {//-----------------���� ������
char number[l_number];//����� ����������
Fine* beg;//��������� �� ������ ������ ���������
Node* left;//��������� �� ����� ���������
Node* right;//��������� �� ������ ���������
};

struct Data {//-----------------�������� ������
char number[l_number];//����� ����������
char time[l_time];//����� ���������
char marka[l_marka];
char type[l_type];//��� ���������
float price;//������ ������
};

Node *descent( Node* p );
Node *first( Data data );
Data input( Action action );
int menu();
void print_node ( const Node& node );
void print_dbase( Node* p );
Node* read_dbase( char* filename );
int read_fine( ifstream &f, Data& data );
int remove_fine( Node* p, const Data& data );
void remove_fines( Node* p );
Node* remove_node( Node* root,
Node* p, Node* parent, Dir dir );
Node* remove_tree( Node* p );
Node* search_insert( Node* root, const Data& data, Action action, Dir& dir, Node*& parent );
void write_dbase( ofstream &f, const Node* root );
void write_node( ofstream &f, const Node& node );
void print_all(Node* root);

int main() { //-----------------------------������� �������--------------------
	setlocale(LC_ALL, "Russian");
	Node* p, *parent;
	Node* root = read_dbase( filename );
	ofstream fout;
	Dir dir;
	g:while ( true ) {
		switch ( menu() ) {
			case 1:// ���� �������� � ���������
				if ( !root ) root = first( input( INSERT ) );
				else search_insert( root, input( INSERT ), INSERT, dir, parent );
				break;

			case 2: {// ���� �������� �� ������ ������
				if ( !root ) { cout << "���� �����" << endl; break; }
				Data data = input( DEL );
				if ( !( p = search_insert( root, data, DEL, dir, parent ) ) )
					cout << "�������� �� �/� ����������� " << endl;
				else if ( remove_fine( p, data ) == 2 ) // ������� ��� ���������
				         root = remove_node( root, p, parent, dir);
				break;
  			}
			case 3:// �������
				if ( !root ) { cout << "���� �����" << endl; break; }
				if ( !(p = search_insert( root, input(INFO), INFO, dir, parent )))
 					cout << "�������� ����������� " << endl;
				else
					print_node( *p );
 				break;
			case 4:	// �����
				fout.open( filename );
 				if ( !fout.is_open() ) {
 					cout << "������ �������� ����� " << filename; return 1;
				}
 				write_dbase( fout, root );
 				return 0;
			case 5: {cout<<"������� ������:";int temp; cin>>temp; if (temp==password) {cout<<"�������� �����������!"<<endl;Node *temp=root; print_all(temp);break;}else{cout<<"�������� ������!"<<endl;goto g;} }			// �������
			default: cout << " ���� ������� ����� �� 1 �� 5" << endl; break;
		}
	}
}
void print_all(Node* temp){
      if ( temp ) { cout<<"Number:"<<temp->number<<endl;
        print_node( *temp );
        cout<<endl;
        print_all(temp->left);
        print_all(temp->right);}
        //print_dbase(p);
}
Node* descent( Node* p ) {// -----------------------------����� �� ������
Node* prev, *y = p->right;
if ( !y->left)
   y->left=p->left;// 1
else {
   do {
        prev = y; y = y->left;}// 2
   while( y->left);
   y->left= p->left;// 3
   prev->left= y->right;// 4
   y->right= p->right;// 5
}
return y;
}

Node* first( Data data ) {// -------------������������ ��������� �������� ������
// �������� ������ � ���������:
   Fine* beg = new Fine;
   strncpy( beg->time,data.time, l_time );
   strncpy( beg->type,data.type, l_type );
   strncpy(beg->marka,data.marka,l_marka);
   beg->price= data.price;
   beg->next= 0;// �������� ������� ���� ������:
   Node* root = new Node;
   strncpy( root->number,data.number, l_number );
   root->beg= beg;root->left=root->right= 0;
   return root;
}

Data input( Action action ) { // ---------------------------------���� ���������
   Data data;
   char buf[10], temp1[3], temp2[3];
   int day, month, hour, min;
   cout << "������� ����� �/�" << endl;
   cin.getline( data.number, l_number ); if ( action == INFO ) return data;
   do { cout << "������� ���� ��������� � ������� ��.��.��:" << endl; cin >> buf;
       strncpy( temp1, buf, 2 ); strncpy( temp2, &buf[3], 2 ); day = atoi( temp1 ); month = atoi( temp2 );
   }while ( !( day > 0 && day < 32 && month > 0 && month < 13 ) ); strcpy( data.time, buf ); strcat( data.time, " " );
   do { cout << "������� ����� ��������� � ������� ��:�� :" << endl; cin >> buf;
       strncpy( temp1, buf, 2 ); strncpy( temp2, &buf[3], 2 ); hour = atoi( temp1 ); min = atoi( temp2 );
   }while ( !( hour >= 0 && hour < 24 && min >= 0 && min < 60 ) ); strcat( data.time, buf );
   cin.get();
   if ( action == DEL ) return data;
       cout << "������� ��� ��������� type" << endl; cin.getline( data.type, l_type );
       cout << "������� ����� ����������" << endl; cin.getline( data.marka, l_marka );
   do { cout << "������� ������ ������:" << endl; cin >> buf;
   }while ( !( data.price = ( float )atof( buf ) ) );
   cin.get();

   return data;
}

int menu() {// -------------------------------------------����� ����
   char buf[10];
   int option;
   do { cout << "==================================" << endl; cout << "1 - ���� �������� � ���������" << endl; cout << "2 - ���� �������� �� ������ ������" << endl; cout << "3 - �������" << endl;
      cout << "4 - �����" << endl; cout<<"5 - �������� ���� ��������� ����"<<endl;
      cout << "==================================" << endl; cin >> buf; option = atoi( buf );
   } while ( !option );
   cin.get();
   return option;
}

void print_node( const Node& node ) { // --------����� �� ����� �������� �� �/� cout << "����� �/�: " << node.number << endl;
   Fine* pf = node.beg; float summa = 0;
   cout<<"����� ����������:"<<pf->marka<<endl;
   while ( pf ) {
      cout << "��� ���������: " << pf->type<< endl; cout << "���� � �����: " <<pf->time;
      cout << " ������ ������: " << pf->price<< endl; summa +=pf->price;
      pf = pf->next;
   }
   cout << "����� ����� �������: " << summa << endl;
}

void print_dbase( Node *p ) {// --------------------����� �� ����� ����� ������
   if ( p ) {
      print_node( *p );
      print_dbase ( p->left);
      print_dbase ( p->right);
   }
}

Node * read_dbase ( char* filename ) { //-------------------������ ���� �� �����
     Node *parent;
     Dir dir;
     Data data;
     ifstream f( filename, ios::in);
     if ( !f ) { cout << "��� ����� " << filename << endl; return NULL; }
     f.getline( data.number, l_number );// ����� �/�
     if( f.eof( ) ) { cout << "������ ���� ( 0 ���� )" << endl; return NULL; }
     read_fine( f, data );  // ������ ���������
     Node* root = first( data );  // ������������ ����� ������
     while ( !read_fine( f, data ) )  // ����������� ���������
         search_insert( root, data, INSERT, dir, parent );  // ������������ ������:
     while ( f.getline( data.number, l_number ) ) {// ����� �/�
         read_fine( f, data ); // ������ ���������
         search_insert( root, data, INSERT, dir, parent );
         while ( !read_fine( f, data ) ) // ����������� ���������
            search_insert( root, data, INSERT, dir, parent );
     }

     return root;
}
// -----------------------------------------������ �� ����� ���������� � ���������
int read_fine( ifstream &f, Data& data ) {
   f.getline( data.time, l_time );
   if( data.time[0] == '=' ) return 1;// ����� ������ ���������
   f.getline( data.type, l_type );
   f.getline(data.marka,l_marka);
   f >> data.price;
   f.get();
   return 0; // ��������� ������� �������
}

// ---------------------------------------------------�������� ��������� �� ������
int remove_fine( Node* p, const Data& data ) {
   Fine* prev, *pf = p->beg;
   bool found = false;
   while ( pf && !found ) {// ���� �� ������ ���������
       if( !strcmp( pf->time,data.time ) ) found = true; // ��������� �������
       else { prev = pf; pf = pf->next;}// ������� � ����. �������� ������
   }
   if ( !found ) { cout << "�������� � ��������� �����������." << endl; return 1; }
   if ( pf == p->beg)
       p->beg=pf->next; //�������� �� ������ ������
   else
       prev->next=pf->next; //�������� �� �������� ��� ����� ������
   delete pf;// ������������ ������ ��-�����������
   if ( !p->beg) return 2;// ������ ����
   return 0;
}

// -----------------------------------------------------------�������� ���� ������
Node* remove_node( Node* root, Node* p, Node* parent, Dir dir ) {
    Node *y;// ����, ���������� ���������
   if ( !p->left)
      y = p->right;// 11
   else if ( !p->right)
      y = p->left;// 12
        else
           y = descent( p );// 13
   if ( p == root ) root = y;// 14
   else { // 15
      if ( dir == LEFT )
          parent->left= y;
      else
          parent->right= y;
   }
   delete p;// 16
   return root;
}

// -------------------------------------------------------------����� � ����������
Node* search_insert( Node* root, const Data& data, Action action,
   Dir& dir, Node*& parent ) {
   Node* p = root;// ��������� �� ������� �������
   bool found = false;// ������� ��������� ������
   int cmp;// ��������� ��������� ������
   parent = 0;
   while ( p && !found ) {// ���� ������ �� ������
      cmp= strcmp( data.number, p->number);// ��������� ������
      if ( !cmp )
         found = true; // �����!
      else {
           parent = p;
           if ( cmp < 0 ) {
              p = p->left;
              dir = LEFT; }// ����� �����
           else {
              p = p->right;dir = RIGHT; }// ����� ������
      }
   }
   if ( action != INSERT ) return p;    // �������� ������ � ���������:
   Fine* pf = new Fine;
   strncpy( pf->time,data.time, l_time );
   strncpy( pf->type,data.type, l_type );
   strncpy(pf->marka,data.marka,l_marka);
   pf->price= data.price;
   pf->next= 0;
   if ( !found ) { // �������� ������ ����:
      p = new Node;
      strncpy( p->number,data.number, l_number );
      p->beg= pf; p->left=p->right= 0;
      if (dir == LEFT)
          parent->left= p;// �������. � ������ ��������� ������
      else parent->right= p;// ������������� � ������� ��������� ������
   }
   else {// ���� ����������, ���������� ��������� � ������
      Fine* temp = p->beg;
      while ( temp->next) temp =temp->next;// ����� ����� ������
         temp->next= pf;
   }
   return p;
}

void write_dbase( ofstream &f, const Node *p ) { //--------------����� ���� � ����
   if ( p ) {
      write_node( f, *p ); write_dbase ( f, p->left ); write_dbase ( f, p->right );
   }
}

void write_node( ofstream &f, const Node& node ) { // - ����� � ���� �������� �� �/�
   f << node.number << endl;
   Fine* pf= node.beg;
   while( pf ) {
      f <<pf->time<< endl <<pf->type<< endl<<pf->marka<<endl <<pf->price<< endl;
      pf = pf->next;
   }
   f << "="<< endl;// ������� ��������� �������� �� �/�
}
