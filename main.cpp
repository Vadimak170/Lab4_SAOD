#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <clocale>
using namespace std;

char* filename = "dbase.txt";
enum Action { INSERT, DEL, INFO };
enum Dir { LEFT, RIGHT };
const int l_time = 20, l_type = 40, l_number = 12;
struct Fine {//---------------Штраф (элемент списка)
char time[l_time];//Время нарушения
char type[l_type];//Вид нарушения
float price;//Размер штрафа
Fine* next;//Указатель на следующий элемент
};

struct Node {//-----------------Узел дерева
char number[l_number];//Номер автомобиля
Fine* beg;//Указатель на начало списка нарушений
Node* left;//Указатель на левое поддерево
Node* right;//Указатель на правое поддерево
};

struct Data {//-----------------Исходные данные
char number[l_number];//Номер автомобиля
char time[l_time];//Время нарушения
char type[l_type];//Вид нарушения
float price;//Размер штрафа
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

int main() { //-----------------------------Главная функция--------------------
	setlocale(LC_ALL, "Russian");
	Node* p, *parent;
	Node* root = read_dbase( filename );
	ofstream fout;
	Dir dir;
	while ( true ) {
		switch ( menu() ) {
			case 1:// Ввод сведений о нарушении
				if ( !root ) root = first( input( INSERT ) );
				else search_insert( root, input( INSERT ), INSERT, dir, parent );
				break;

			case 2: {// Ввод сведений об оплате штрафа
				if ( !root ) { cout << "База пуста" << endl; break; }
				Data data = input( DEL );
				if ( !( p = search_insert( root, data, DEL, dir, parent ) ) )
					cout << "Сведения об а/м отсутствуют " << endl;
				else if ( remove_fine( p, data ) == 2 ) // Удалены все нарушения
				         root = remove_node( root, p, parent, dir);
				break;
  			}
			case 3:// Справка
				if ( !root ) { cout << "База пуста" << endl; break; }
				if ( !(p = search_insert( root, input(INFO), INFO, dir, parent )))
 					cout << "Сведения отсутствуют " << endl;
				else
					print_node( *p );
 				break;
			case 4:	// Выход
				fout.open( filename );
 				if ( !fout.is_open() ) {
 					cout << "Ошибка открытия файла " << filename; return 1;
				}
 				write_dbase( fout, root );
 				return 0;
			case 5: print_dbase( root ); break;			// Отладка
			default: cout << " Надо вводить число от 1 до 4" << endl; break;
		}
	}
}

Node* descent( Node* p ) {// -----------------------------Спуск по дереву
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

Node* first( Data data ) {// -------------Формирование корневого элемента дерева
// Создание записи о нарушении:
   Fine* beg = new Fine;
   strncpy( beg->time,data.time, l_time );
   strncpy( beg->type,data.type, l_type );
   beg->price= data.price;
   beg->next= 0;// Создание первого узла дерева:
   Node* root = new Node;
   strncpy( root->number,data.number, l_number );
   root->beg= beg;root->left=root->right= 0;
   return root;
}

Data input( Action action ) { // ---------------------------------Ввод нарушения
   Data data;
   char buf[10], temp1[3], temp2[3];
   int day, month, hour, min;
   cout << "Введите номер а/м" << endl;
   cin.getline( data.number, l_number ); if ( action == INFO ) return data;
   do { cout << "Введите дату нарушения в формате ДД.ММ.ГГ:" << endl; cin >> buf;
       strncpy( temp1, buf, 2 ); strncpy( temp2, &buf[3], 2 ); day = atoi( temp1 ); month = atoi( temp2 );
   }while ( !( day > 0 && day < 32 && month > 0 && month < 13 ) ); strcpy( data.time, buf ); strcat( data.time, " " );
   do { cout << "Введите время нарушения в формате ЧЧ:ММ :" << endl; cin >> buf;
       strncpy( temp1, buf, 2 ); strncpy( temp2, &buf[3], 2 ); hour = atoi( temp1 ); min = atoi( temp2 );
   }while ( !( hour >= 0 && hour < 24 && min >= 0 && min < 60 ) ); strcat( data.time, buf );
   cin.get();
   if ( action == DEL ) return data;
       cout << "Введите тип нарушения type" << endl; cin.getline( data.type, l_type );
   do { cout << "Введите размер штрафа:" << endl; cin >> buf;
   }while ( !( data.price = ( float )atof( buf ) ) );
   cin.get();
   return data;
}

int menu() {// -------------------------------------------Вывод меню
   char buf[10];
   int option;
   do { cout << "==================================" << endl; cout << "1 - Ввод сведений о нарушении" << endl; cout << "2 - Ввод сведений об оплате штрафа" << endl; cout << "3 - Справка" << endl;
      cout << "4 - Выход" << endl;
      cout << "==================================" << endl; cin >> buf; option = atoi( buf );
   } while ( !option );
   cin.get();
   return option;
}

void print_node( const Node& node ) { // --------Вывод на экран сведений об а/м cout << "Номер а/м: " << node.number << endl;
   Fine* pf = node.beg; float summa = 0;
   while ( pf ) {
      cout << "Вид нарушения: " << pf->type<< endl; cout << "Дата и время: " <<pf->time;
      cout << " Размер штрафа: " << pf->price<< endl; summa +=pf->price;
      pf = pf->next;
   }
   cout << "Общая сумма штрафов: " << summa << endl;
}

void print_dbase( Node *p ) {// --------------------Вывод на экран всего дерева
   if ( p ) {
      print_node( *p );
      print_dbase ( p->left);
      print_dbase ( p->right);
   }
}

Node * read_dbase ( char* filename ) { //-------------------Чтение базы из файла
     Node *parent;
     Dir dir;
     Data data;
     ifstream f( filename, ios::in);
     if ( !f ) { cout << "Нет файла " << filename << endl; return NULL; }
     f.getline( data.number, l_number );// Номер а/м
     if( f.eof( ) ) { cout << "Пустой файл ( 0 байт )" << endl; return NULL; }
     read_fine( f, data );  // Первое нарушение
     Node* root = first( data );  // Формирование корня дерева
     while ( !read_fine( f, data ) )  // Последующие нарушения
         search_insert( root, data, INSERT, dir, parent );  // Формирование дерева:
     while ( f.getline( data.number, l_number ) ) {// Номер а/м
         read_fine( f, data ); // Первое нарушение
         search_insert( root, data, INSERT, dir, parent );
         while ( !read_fine( f, data ) ) // Последующие нарушения
            search_insert( root, data, INSERT, dir, parent );
     }

     return root;
}
// -----------------------------------------Чтение из файла информации о нарушении
int read_fine( ifstream &f, Data& data ) {
   f.getline( data.time, l_time );
   if( data.time[0] == '=' ) return 1;// Конец списка нарушений
   f.getline( data.type, l_type );
   f >> data.price;
   f.get();
   return 0; // Нарушение считано успешно
}

// ---------------------------------------------------Удаление нарушения из списка
int remove_fine( Node* p, const Data& data ) {
   Fine* prev, *pf = p->beg;
   bool found = false;
   while ( pf && !found ) {// Цикл по списку нарушений
       if( !strcmp( pf->time,data.time ) ) found = true; // Нарушение найдено
       else { prev = pf; pf = pf->next;}// Переход к след. элементу списка
   }
   if ( !found ) { cout << "Сведения о нарушении отсутствуют." << endl; return 1; }
   if ( pf == p->beg)
       p->beg=pf->next; //Удаление из начала списка
   else
       prev->next=pf->next; //Удаление из середины или конца списка
   delete pf;// Освобождение памяти из-подэлемента
   if ( !p->beg) return 2;// Список пуст
   return 0;
}

// -----------------------------------------------------------Удаление узла дерева
Node* remove_node( Node* root, Node* p, Node* parent, Dir dir ) {
    Node *y;// Узел, заменяющий удаляемый
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

// -------------------------------------------------------------Поиск с включением
Node* search_insert( Node* root, const Data& data, Action action,
   Dir& dir, Node*& parent ) {
   Node* p = root;// Указатель на текущий элемент
   bool found = false;// Признак успешного поиска
   int cmp;// Результат сравнения ключей
   parent = 0;
   while ( p && !found ) {// Цикл поиска по дереву
      cmp= strcmp( data.number, p->number);// Сравнение ключей
      if ( !cmp )
         found = true; // Нашли!
      else {
           parent = p;
           if ( cmp < 0 ) {
              p = p->left;
              dir = LEFT; }// Спуск влево
           else {
              p = p->right;dir = RIGHT; }// Спуск вправо
      }
   }
   if ( action != INSERT ) return p;    // Создание записи о нарушении:
   Fine* pf = new Fine;
   strncpy( pf->time,data.time, l_time );
   strncpy( pf->type,data.type, l_type );
   pf->price= data.price;
   pf->next= 0;
   if ( !found ) { // Создание нового узла:
      p = new Node;
      strncpy( p->number,data.number, l_number );
      p->beg= pf; p->left=p->right= 0;
      if (dir == LEFT)
          parent->left= p;// Присоед. к левому поддереву предка
      else parent->right= p;// Присоединение к правому поддереву предка
   }
   else {// Узел существует, добавление нарушения в список
      Fine* temp = p->beg;
      while ( temp->next) temp =temp->next;// Поиск конца списка
         temp->next= pf;
   }
   return p;
}

void write_dbase( ofstream &f, const Node *p ) { //--------------Вывод базы в файл
   if ( p ) {
      write_node( f, *p ); write_dbase ( f, p->left ); write_dbase ( f, p->right );
   }
}

void write_node( ofstream &f, const Node& node ) { // - Вывод в файл сведений об а/м
   f << node.number << endl;
   Fine* pf= node.beg;
   while( pf ) {
      f <<pf->time<< endl <<pf->type<< endl <<pf->price<< endl;
      pf = pf->next;
   }
   f << "="<< endl;// Признак окончания сведений об а/м
}
