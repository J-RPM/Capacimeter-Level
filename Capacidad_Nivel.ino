/*
                     *** Nivel de Gasoil en un depósito de 1000 litros ***
  La librería "CapacitorLite.h" utiliza menos almacenamiento de programa y es más rápida que la versión "Capacitor.h"
                        "CapacitorLite.h" solo puede medir entre 0,2pF y 655pF
      Measure() entrega valores x100, así habrá que dividir el resultado entre 100, para prersentar pF con 2 decimales 
                  https://wordpress.codewrite.co.uk/pic/2014/01/21/cap-meter-with-arduino-uno/

                  "Capacitor.h" permite medir capacidades comprendidas entre 1pF y 100uF
                  https://wordpress.codewrite.co.uk/pic/2014/01/25/capacitance-meter-mk-ii/
    
_____________________________________________________________________________________________________
                                       Escrito por: J_RPM
                                        http://j-rpm.com
                                     Noviembre de 2022 (v1.2)
________________________________________________________________________________________________________
*/
#include <CapacitorLite.h>
#include <Capacitor.h>
#include <LiquidCrystal.h>

// Definición de los dos pines de conexión (D12, A2), el mismo para las dos librerías
// Si el condensador bajo prueba es electrolítico el Pin + debe conectarse a D12
CapacitorLite capL(12,A2);
Capacitor capH(12,A2);

#define  muestras 150      // Número de muestras consecutivas, para promediar la medida
int nivel[5];              // Dimensiona el array con 5 valores (máximo)
int minimo=25200;          
int maximo=27333;          
float medida;
int medidaD; 
String unidad;
String Version = "(v1.2)";

// Inicializa el display LCD con sus pines de conexión (RS,E,DB4,DB5,DB6,DB7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Define los carácteres gráficos RAM
  byte nivel_0[8]= {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
  };
  byte nivel_1[8]= {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B00000,
  };
  byte nivel_2[8]= {
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B00000,
  };
  byte nivel_3[8]= {
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B00000,
  };
  byte nivel_4[8]= {
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B00000,
  };
  byte nivel_5[8]= {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000,
  };

void setup() {
  Serial.begin(9600);
  // Estructura de las variables de calibrado y sus valores por defecto
  // ... para ATMEGA328P con la librería: Capacitor.h 
  //void Capacitor::Calibrate(float strayCap, float pullupRes)
  //#define STRAY_CAP (26.30);
  //#define R_PULLUP (34.80);
  capH.Calibrate(44.80,36.80);  // J_RPM: 44.80,36.8 >>> Se puede comentar esta línea, si los valores C,R son 26.30 y 34.80
  
  // Estructura de las variables de calibrado y sus valores por defecto
  // ... para ATMEGA328P con la librería: CapacitorLite.h 
  //void CapacitorLite::Calibrate(unsigned int strayCap)
  capL.Calibrate(4480);     // J_RPM: 4480 >>> Se puede comentar esta línea, si el valor C es 2630

  // Almacena los caracteres gráficos en la RAM
  lcd.createChar(0, nivel_0);
  lcd.createChar(1, nivel_1);
  lcd.createChar(2, nivel_2);
  lcd.createChar(3, nivel_3);
  lcd.createChar(4, nivel_4);
  lcd.createChar(5, nivel_5);
  
  // Define el tamaño del display
  lcd.begin(16, 2);
  
  // Mensaje inicial
  Serial.print(F("### Depósito + Capacímetro J_RPM "));  
  Serial.print(Version);  
  Serial.println(F(" ###"));  
  
  lcd.setCursor(0, 0);
  lcd.print(F(" NIVEL DEPOSITO "));
  lcd.setCursor(5, 1);
  lcd.print(Version);
 
  delay(1000); 
}

void loop() {
  // Habilitar para mostrar los valores ADC por el puerto serie
  //capH.ShowDebug(true);
  //capL.ShowDebug(true);
 
  medida = capL.Measure();

  //Comprueba si es una medida válida, inferior a 355pF >>> Nivel del depósito
  if (medida < 35500) {
      Serial.println(F("----------------"));  
      Serial.print(F("Medida: "));  
      Serial.println(medida);  

      mediaL();               // Promedia la medida: CapacitorLite.h (pF x100)
      medidaD = medida;       // Carga la medida 'float' para mostrar el valor sin decimales
      
      //Gráfica del Nivel
      int nivel = map(medidaD, minimo, maximo, 0, 80); 
      int porcentaje = (nivel*100) / 80;

      if (nivel <= 0) {
        porcentaje=0;
      }else if (porcentaje >= 100) {
        porcentaje=100;
      }

      Serial.print(F("Promedio: "));  
      Serial.println(medidaD);  
      Serial.print(F("Mínimo: "));  
      Serial.println(minimo);  
      Serial.print(F("Máximo: "));  
      Serial.println(maximo);  
      Serial.print(F("Nivel: "));  
      Serial.println(nivel);  
      Serial.print(F("Porcentaje: "));  
      Serial.println(porcentaje);  

      for (int i = 0; i < 16; i++) {
        lcd.setCursor(i, 0);
        ponNivel(nivel);
        nivel = nivel-5;
      }
    
      // Muestra los resultados de la medida
      lcd.setCursor(0, 1);
      lcd.print("*   ");
      lcd.setCursor(4, 1);
      lcd.print(medidaD);  
      lcd.print(F("   "));  
      lcd.print(porcentaje);  
      lcd.print(F("%      "));  
    
    }else {
      // Lee la capacidad en pF (356pF...100uF)
      mediaH();           // Promedia la medida: Capacitor.h
      lcd.setCursor(0, 0);
      lcd.print(F(">>> Capacitor.h "));
      Serial.println(F("### Capacitor.h ###"));  
      if (medida >= 101000000) {
         medida = -1;
      }else if (medida >= 1000000) {
         medida = medida/1000000;
         unidad =" uF.";
      }else if (medida >= 1000) {
         medida = medida/1000;
         unidad =" nF.";
      }else {
        unidad =" pF.";
      }
    
      // Presenta los resultados de la medida y muestra la actividad en el display
      lcd.setCursor(0, 1);
      lcd.print("*    ");
      lcd.setCursor(5, 1);
      if (medida >= 0) {
        Serial.print(medida);  
        Serial.println(unidad);  
        lcd.print(medida);
        lcd.print(unidad);
      }else {
        Serial.println(F(">100 uF."));  
        lcd.print(F(">100 uF."));
      }
      lcd.print(F("         "));
      
    }
  //Mantiene la marca de actividad en el LCD durante 300ms
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(F(" "));

  // Retardo entre dos medidas consecutivas (refresco de los valores en el display)
  delay(300);
}
/**********************************************************************************************************/
/***  Funciones  ***/
/**********************************************************************************************************/
// Gráfico de nivel 
void ponNivel(int valor){
  if (valor > 4) {
     lcd.write (byte(5));
  }else { 
     if (valor < 0){valor=0;}
     lcd.write (byte(valor));
  }
}
/**********************************************************************************************************/
///////////////////////////////////////////////////////
void mediaL(){
  float valorMedio = 0;                       // Reinicia valor medio
  for (byte y = 0; y < muestras; y++){        // Muestreos 
    medida = capL.Measure();
    valorMedio = valorMedio + medida;
    delay(10);
  }
  medida  = valorMedio / muestras;           // Guarda el valor promedio de las muestras
 
  // Corrige las medidas: -25pF 
  if (medida > 2500) {
    medida = medida-2500;
  }else {
    medida = 0;
  }
}
///////////////////////////////////////////////////////
void mediaH(){
  float valorMedio = 0;                       // Reinicia valor medio
  for (byte y = 0; y < muestras; y++){        // Muestreos 
    medida = capH.Measure();
    valorMedio = valorMedio + medida;
    delay(10);
  }
  medida = valorMedio / muestras;             // Guarda el valor promedio de las muestras
  medida = medida-25;
}
///////////////////////////////////////////////////////

// Fin del programa //
