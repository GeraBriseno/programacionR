#include <LiquidCrystal_I2C.h>


//pantalla
int pinSDA = 21;
int pinSCL = 22;
int lcdColumns = 16; 
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  //asume que la direccion I2C de la pantalla es 0x27

//sensores ultrasonicos
const int trigPin1 = 2;
//const int echoPin1 = 4;
const int trigPin2 = 1;
const int echoPin2 = 3;
const int trigPin3 = 17;
const int echoPin3 = 16;
long duracion;
int distancia;

int distanciaSensorAlPiso = 36;       //distancia del sensor al piso en centímetros /////////////////
int distanciaSensorANivelLLeno = 10;   //distancia del sensor al nivel del tanque lleno ///////////////////

//niveles tanques
String niveles = "Niveles ";

// botones inicio 

int boton1 = 4;
int boton2 = 5;
int boton3 = 23;

int estadoBoton1;
int estadoBoton2;
int estadoBoton3;

int ultimoEstadoBoton1 = LOW;
int ultimoEstadoBoton2 = LOW;
int ultimoEstadoBoton3 = LOW;

int menuPrincipal = 1;
int menuRecirculacion = 1;
int menuInyeccion = 1;
int menuLavado = 1;

int menuPrincipalSeleccionado = true;
int menuRecirculacionSeleccionado = false;
int menuInyeccionSeleccionado = false;
int menuLavadoSeleccionado = false;

bool valorBoton1 = false;
bool valorBoton2 = false;
bool valorBoton3 = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
long tiempoRecirculacion = 0;
long tiempoPurgaR = 0;
long tiempoPurgaH = 0;
long tiempoLavado = 0;

bool operacion = true;

//valvulas
int RR =19;  //RECIRCULAR RESINA
int RH =18;   //RECIRCULAR HARDENER
int BHA = 32;  //BOMBA HARDENER ALTA
int BHB =33; //BOMBA HARDENER BAJA
int BRB =25;  //BOMBA RESINA BAJA
int BRA =26; //BOMBA RESINA ALTA
int SR =27;  //SALIDA RESINA
int SH =14;  //SALIDA HARDNER
int SA =12;  //SALIDA AIRE
int BL=13;  //BOMBA AGUA

void setup (){

  pinMode(pinSDA, OUTPUT);
  pinMode(pinSCL, OUTPUT);

  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);
  
  //valvulas
  pinMode(RR, OUTPUT);
  pinMode(RH, OUTPUT);
  pinMode(BHA, OUTPUT);
  pinMode(BHB, OUTPUT);
  pinMode(BRB, OUTPUT);
  pinMode(BRA, OUTPUT);
  pinMode(SR, OUTPUT);
  pinMode(SH, OUTPUT);
  pinMode(SA, OUTPUT);
  pinMode(BL, OUTPUT);    
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(trigPin3, OUTPUT);
  //pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  updateMenu();

  Serial.begin(9600);
}

/////funciones//////////////////////////////////////////////////////////////////////////////////////////////////
void recirculacionEncender(){
  digitalWrite(BRB,HIGH);
  digitalWrite(BHB,HIGH);
  digitalWrite(RR,HIGH); 
  digitalWrite(RH,HIGH);
}
void recirculacionApagar(){
  digitalWrite(BRB,LOW);
  digitalWrite(BHB,LOW);
  digitalWrite(RR,LOW); 
  digitalWrite(RH,LOW);
}
void purgarResina(){
  digitalWrite(BRA,HIGH);
  digitalWrite(SR,HIGH);
  digitalWrite(SA,HIGH); 
  }
void purgarHardener(){
  digitalWrite(BHA,HIGH);
  digitalWrite(SH,HIGH);
  digitalWrite(SA,HIGH);  
}
void terminarPurgaResina(){
  digitalWrite(BRA,LOW);
  digitalWrite(SR,LOW);
  digitalWrite(SA,LOW); 
}
void terminarPurgaHardener(){
  digitalWrite(BHA,LOW);
  digitalWrite(SH,LOW);
  digitalWrite(SA,LOW);  
}

void lavadoEncender(){
  digitalWrite(RR,HIGH);
  digitalWrite(RH,HIGH);
  digitalWrite(SR,HIGH);
  digitalWrite(SH,HIGH);
  digitalWrite(SA,HIGH);
  digitalWrite(BL,HIGH);  
}
void lavadoApagar(){
  digitalWrite(RR,LOW);
  digitalWrite(RH,LOW);
  digitalWrite(SR,LOW);
  digitalWrite(SH,LOW);
  digitalWrite(SA,LOW);
  digitalWrite(BL,LOW);  
}
void medicionUltrasonico(int trigPin, int echoPin, String x){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duracion = pulseIn(echoPin, HIGH);
  distancia = (duracion * 0.034) / 2;

  float nivelTanque = map(distancia, distanciaSensorAlPiso, distanciaSensorANivelLLeno, 0, 100);
  niveles = niveles + "T"+x+": "+distancia+" ";
  delay(5);
}

void updateMenu(){
  switch(menuPrincipal){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(">Recirculacion");
      lcd.setCursor(1,1);
      lcd.print("Inyeccion");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Recirculacion");
      lcd.setCursor(0,1);
      lcd.print(">Inyeccion");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Inyeccion");
      lcd.setCursor(0,1);
      lcd.print(">Lavado");
      break;       
  }
}

void updateRecirculacion(){
  switch(menuRecirculacion){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(0,1);
      lcd.print(">10 20 30 60 minutos");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(0,1);
      lcd.print("10 >20 30 60 minutos");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(0,1);
      lcd.print("10 20 >30 60 minutos");
      break; 
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(0,1);
      lcd.print("10 20 30 >60 minutos");
      break;       
  }
}

void updateInyeccion(){
    switch(menuInyeccion){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Inyeccion Purgar:");
      lcd.setCursor(0,1);
      lcd.print(">Resina Hardener");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Inyeccion Purgar:");
      lcd.setCursor(0,1);
      lcd.print("Resina >Hardener");
      break;      
  }
}

void updateLavado(){
    switch(menuLavado){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Opciones Lavado");
      lcd.setCursor(0,1);
      lcd.print(">Opcion1 Opcion2");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Opciones Lavado:");
      lcd.setCursor(0,1);
      lcd.print("Opcion1 >Opcion2");
      break;      
  }
}
int terminarOperacion(){
  Serial.println("x");
  int lectura3 = !digitalRead(boton3);
  if(lectura3 == HIGH){
  return 0;
  }
  else{
  return 1;  
  }
}

/////funciones//////////////////////////////////////////////////////////////////////////////////////////////////

void loop(){
  lcd.setCursor(0, 0);
  
  int lectura1 = !digitalRead(boton1);
  int lectura2 = !digitalRead(boton2);
  int lectura3 = !digitalRead(boton3);
  
  Serial.println(lectura1);
  if (lectura1 != ultimoEstadoBoton1 || lectura2 != ultimoEstadoBoton2 || lectura3 != ultimoEstadoBoton3){
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay){

    if(lectura3 != estadoBoton3){
      estadoBoton3 = lectura3;
      if (lectura3 == HIGH) {
        menuLavadoSeleccionado = false;
        menuRecirculacionSeleccionado = false;
        menuInyeccionSeleccionado = false;
        menuPrincipalSeleccionado = true;
        updateMenu();
        }
      }

    
    if (lectura1 != estadoBoton1){
      estadoBoton1 = lectura1;
      if (lectura1 == HIGH) {
        if(menuPrincipalSeleccionado == true){
          switch(menuPrincipal){
            case 1:
              updateRecirculacion();
              menuPrincipalSeleccionado = false;
              menuRecirculacionSeleccionado = true;
              break;
            case 2:
              //inyeccion
              updateInyeccion();
              menuPrincipalSeleccionado = false;
              menuInyeccionSeleccionado = true;
              break;
            case 3:
              //lavado
              updateLavado();
              menuPrincipalSeleccionado = false;
              menuLavadoSeleccionado = true;
              break;
            }
          return;  
          }
        //RECIRCULACION  
        if(menuRecirculacionSeleccionado == true){
          bool activado = true;
          switch(menuRecirculacion){
            case 1:
              tiempoRecirculacion = millis() + 600000;
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                long tRestante = ((tiempoRecirculacion - millis())/60000);                    //tiempo restante de la operacion
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){                                             //limpiar la pantalla cuando sean menos de 2 cifras
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            case 2:
              tiempoRecirculacion = millis() + 1200000;
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                long tRestante = ((tiempoRecirculacion - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            case 3:
              tiempoRecirculacion = millis() + 1800000;
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                long tRestante = ((tiempoRecirculacion - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            case 4:
              tiempoRecirculacion = millis() + 15000;
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                long tRestante = ((tiempoRecirculacion - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            }
            return; 
          }
        //INYECCION  
        if(menuInyeccionSeleccionado == true){
          bool activado = true;
          switch(menuInyeccion){
            case 1:
              //purga Resina
              tiempoPurgaR = millis() + 15000;
              purgarResina();
              lcd.clear();
              while(tiempoPurgaR > millis()){
                long tRestante = ((tiempoPurgaR - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante)+"Res");
              }
              terminarPurgaResina();
              updateInyeccion();
              break;
            case 2:
              //purga Hardener
              tiempoPurgaH = millis() + 15000;
              purgarHardener();
              lcd.clear();
              while(tiempoPurgaH > millis()){
                long tRestante = ((tiempoPurgaH - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante)+"Hard");
              }
              terminarPurgaHardener();
              updateInyeccion();
              break;
            }
            return; 
          }
        //LAVADO  
        if(menuLavadoSeleccionado == true){
          bool activado = true;
          switch(menuLavado){
            case 1:
              tiempoLavado = millis() + 15000;
              lavadoEncender();
              lcd.clear();
              while(tiempoLavado > millis()){
                long tRestante = ((tiempoLavado - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              lavadoApagar();
              menuLavadoSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            case 2:
              tiempoLavado = millis() + 20000;
              lavadoEncender();
              lcd.clear();
              while(tiempoLavado > millis()){
                long tRestante = ((tiempoLavado - millis())/1000);
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              lavadoApagar();
              menuLavadoSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            }
            return; 
          }
    }
    }
  
    if (lectura2 != estadoBoton2){
      estadoBoton2 = lectura2;
      if(lectura2 == HIGH) {
          if(menuPrincipalSeleccionado == true){
            if(menuPrincipal <= 2){
              menuPrincipal++;
              updateMenu();

            }
            else{
              menuPrincipal = 1;
              updateMenu();
            }
          }
      if(menuRecirculacionSeleccionado == true){
          if(menuRecirculacion <= 3){
            menuRecirculacion++;
            updateRecirculacion();
            }
          else{
            menuRecirculacion = 1;
            updateRecirculacion();
            }
          }
        if(menuInyeccionSeleccionado == true){
          if(menuInyeccion == 1){
            menuInyeccion++;
            updateInyeccion();
            }
          else{
            menuInyeccion--;
            updateInyeccion();
            }
          }
        if(menuLavadoSeleccionado == true){
          if(menuLavado == 1){
            menuLavado++;
            updateLavado();
            }
          else{
            menuLavado--;
            updateLavado();
            }
          }
        }
    }
  }
  
  ultimoEstadoBoton1 = lectura1;
  ultimoEstadoBoton2 = lectura2;
  ultimoEstadoBoton3 = lectura3;
}
