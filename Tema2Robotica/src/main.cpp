#include <Arduino.h>

const int ledR = 6;   
const int ledG = 5;  
const int ledB = 4;   
const int butonStart = 2;         
const int butonDificultate = 3;     

// Variabile de stare
bool jocActiv = false;           
int dificultate = 0;                
unsigned long timpStart = 0;       
int cuvinteCorecte = 0;              
unsigned long interval = 5000;     
unsigned long ultimulTimpCuvant = 0;    
unsigned long timpNumaratoare = 3000; 

// Dictionar de cuvinte
const String cuvinte[] = {"robot", "arduino", "senzor", "cod", "motor"};
const int numarCuvinte = 5;
String cuvantCurent = "";            
String inputUtilizator = "";             

// Functii
void afiseazaCuvantAleator();
void seteazaCuloareLED(int r, int g, int b);
void numaratoareInversa();
void incepeJoc();
void opresteJoc();
void schimbaDificultate();
void verificaInputUtilizator();

// Setarea culorii LED-ului RGB
void seteazaCuloareLED(int r, int g, int b) {
    analogWrite(ledR, r);
    analogWrite(ledG, g);
    analogWrite(ledB, b);
}

// Numaratoare inversa
void numaratoareInversa() {
    for (int i = 3; i > 0; i--) {
        Serial.println(i);
        seteazaCuloareLED(255, 255, 255); 
        delay(1000);
    }
}

// Inceperea jocului
void incepeJoc() {
    numaratoareInversa(); 
    jocActiv = true;
    timpStart = millis();
    ultimulTimpCuvant = millis();
    cuvinteCorecte = 0;
    Serial.println("Runda a inceput!");
    seteazaCuloareLED(0, 255, 0);  
    afiseazaCuvantAleator();  
}

// Oprirea jocului
void opresteJoc() {
    jocActiv = false;
    Serial.print("Cuvinte corecte: ");
    Serial.println(cuvinteCorecte);
    Serial.println("Runda s-a oprit!");
    seteazaCuloareLED(255, 255, 255);  
}

// Schimbarea dificultatii
void schimbaDificultate() {
    dificultate = (dificultate + 1) % 3;
    if (dificultate == 0) {
        Serial.println("Easy");
        interval = 5000;  
    } else if (dificultate == 1) {
        Serial.println("Medium");
        interval = 3000; 
    } else {
        Serial.println("Hard mode on!");
        interval = 2000;  
    }
}

// Afisarea unui cuvant aleatoriu
void afiseazaCuvantAleator() {
    int index = random(numarCuvinte); 
    cuvantCurent = cuvinte[index];     
    Serial.println(cuvantCurent);    
    ultimulTimpCuvant = millis();         
}

// Verificarea intrarii utilizatorului
void verificaInputUtilizator() {
    static unsigned long ultimulTimpCitire = 0; 
    unsigned long currentMillis = millis();

    // Verificam daca a trecut un timp suficient intre citiri
    if (currentMillis - ultimulTimpCitire > 50) {  
        static char ultimulCaracterProcesat = '\0'; 
        while (Serial.available() > 0) { 
            char inputChar = Serial.read();  

            if (inputChar != '\n' && inputChar != '\r') {
                // Procesam doar daca caracterul este diferit de ultimul caracter procesat
                if (inputChar != ultimulCaracterProcesat) {
                    ultimulCaracterProcesat = inputChar; 

                    if (inputChar == '\b') { 
                        // Stergem ultimul caracter
                        if (inputUtilizator.length() > 0) {
                            inputUtilizator.remove(inputUtilizator.length() - 1);
                            Serial.print("\b \b");  
                        }
                    } else {
                        // Adaugam caracterul introdus
                        if (inputUtilizator.length() < cuvantCurent.length()) {  
                            inputUtilizator += inputChar;
                            Serial.print(inputChar);  
                        }
                    }
                }
            }
        }

        // Resetam caracterul precedent dupa procesare
        ultimulTimpCitire = currentMillis;
    }

    // Verificam daca cuvantul introdus este corect
    if (inputUtilizator == cuvantCurent) {
        cuvinteCorecte++;
        inputUtilizator = "";  
        Serial.println("\nCuvant corect!");
        seteazaCuloareLED(0, 255, 0);  
        afiseazaCuvantAleator();  
    } else if (currentMillis - ultimulTimpCuvant >= interval) {
        // Verificam daca timpul a expirat
        Serial.println("\nTimpul a expirat pentru acest cuvant!");
        seteazaCuloareLED(255, 0, 0);
        inputUtilizator = "";  
        afiseazaCuvantAleator();  
    } else if (inputUtilizator.length() > 0 && inputUtilizator != cuvantCurent) {
        // Daca utilizatorul a introdus un cuvant gresit
        seteazaCuloareLED(255, 0, 0); 
    }
}

void setup() {
    pinMode(ledR, OUTPUT);
    pinMode(ledG, OUTPUT);
    pinMode(ledB, OUTPUT);
    pinMode(butonStart, INPUT_PULLUP);
    pinMode(butonDificultate, INPUT_PULLUP);
    Serial.begin(9600);

    seteazaCuloareLED(255, 255, 255);
}

void loop() {
    // Butonul de start pentru inceperea/opririi jocului
    if (digitalRead(butonStart) == LOW) {
        delay(200);
        if (!jocActiv) {
            incepeJoc();
        } else {
            opresteJoc();
        }
    }

    // Butonul de dificultate doar in repaus
    if (!jocActiv && digitalRead(butonDificultate) == LOW) {
        delay(200);  
        schimbaDificultate();
    }

    // Verificam daca jocul este activ
    if (jocActiv) {
        unsigned long currentMillis = millis();
        
        // Cronometru pentru durata rundei
        if (currentMillis - timpStart >= 30000) {  
            opresteJoc();
        }

        verificaInputUtilizator(); 
    }
}
