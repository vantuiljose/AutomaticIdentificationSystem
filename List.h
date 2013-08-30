 

struct TTrajetoria{
	  int id;
      int x;
      int y;
      int area;
	  unsigned int ttl;
	  int dx;
	  unsigned int recognized;
	  int inicio;
	  int fim;
	  int toqueBordas;
	  boolean isTouchingBorder;
	  //IplImage[5] listaImagens; 
	  
	 struct TTrajetoria *prox;
   } ;
 typedef struct TTrajetoria Trajetoria; 

 //Trajetoria trajs[TRAJ_MAX];

 struct TTrajetoriasAtivas{
	 Trajetoria* ini;
	 Trajetoria* fin;
	 unsigned int size;
 };
 typedef struct TTrajetoriasAtivas TrajetoriasAtivas;

void addTraj(TrajetoriasAtivas* tAtivas, Trajetoria* traj);

void createListTraj(TrajetoriasAtivas** tAtivas);