/**
 * WEBSERVER_FAN-CONTROLLER
 * 
 * Lo sketch si connette alla rete WiFi
 * e avvia il web server.
 * 
 * A questo punto l'utente, avendo il dispositivo connesso
 * all stessa rete WiFi del nodeMCU, puo' visitare la pagina
 * del web server per attuare un'uscita.
 * 
 * In questo caso e' prevista l'attuazione di una ventola
 * ma e' possibile utilizzare lo sketch per qualsiasi
 * circuito attuabile da 3.3 volt.
 * 
 * @file
 * @author Martino Morini
 * @author Luca Fraccari
 * @author Zenaro Stefano
 * @date $Date: 2020-04-02 17:00:00 $
 * @version 01_01
*/

// dim(x) restituisce la dimensione di un array
template <typename T, int N> char(&dim_helper(T(&)[N]))[N];
#define dim(x) (sizeof(dim_helper(x)))

// ***************************************************************
//
//                            LIBERIE
//
// ***************************************************************

/// libreria WiFi per le schede basate sul microcontrollore ESP8266
#include <ESP8266WiFi.h>

// ***************************************************************
//
//                            VARIABILI
//
// ***************************************************************

/// Variabile di debug (true = visualizza messaggi di debug)
const bool boolDebug = true;

/// SSID della rete a cui connettersi
const char* ssid     = "SSID";
/// Password della rete a cui connettersi
const char* password = "PASSWORD";

/// Stato dell'uscita
String outState = "off";
/// Pin uscita
const int outPin = 5;

/// CSS tag html: usa font helvetica, visualizza in linea (elemento contenente occupa tutta width), aggiungi margine laterale, centra testo
const String htmlCSS = "html, body { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
/// CSS tag pulsanti: togli il bordo, usa come colore del testo il bianco, aggiungi padding per centrare testo/aumentare la dimensione del pulsante rispetto al solo testo, 
///                   togli lo stile del testo dei link dal pulsante, metti dimensione font = 30px, margine e metti come cursore il "pointer"
const String btnCSS = ".button { border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
/// CSS tag pulsante off: il pulsante serve per spegnere output (imposta colore grigio)
const String offBtnCSS = ".off { background-color: #77878A; }";
/// CSS tag pulsante on: il pulsante serve per accendere output (imposta colore blu)
const String onBtnCSS = ".on { background-color: #195B6A; }";

/// Immagine on Base64
const char onImg[][105] = {"data:image/png;base64,R0lGODlhgQCoAPY/AP7+/vLo39Pr2bzdw/ny7bXavKnYte7l1MbdxOj06svhyvz49dbhzfr18dfY1+Dk",
"0/38+tzu4evm2fXt5srn0q7ZufP69P769/r6+Pbv6vn8+PTq4u738P39/e7o2/fr5eLizqvXt67Wt+bl",
"1vDn3PXr4vv8+/fu5/b5987OzO3t7OHp34qjld7x4n+cj/j8+/Hl17bewd3h36jXs//////+/L/OxPXo",
"4PDk1Obn5vHz8srX0Ku7r+ri0WuNf/Pp4P37/Huai22PgYnAmHOThKDRrPHl1fLq5eDo1qjWspbJo6LK",
"qfTr3vLn2/Hp2/Dk1pWsoWyOgPLs4PDo2O/m1/Tn3qjXtc3MyqXXstXVxrfGvuHb0nWVh/Lm17rBsniX",
"iPHq3XeTg4GZibPQsnCQgQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH/C05FVFNDQVBFMi4wAwEAAAAh/wtY",
"TVAgRGF0YVhNUDw/eHBhY2tldCBiZWdpbj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlk",
"Ij8+IDx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBD",
"b3JlIDUuNS1jMDIxIDc5LjE1NDkxMSwgMjAxMy8xMC8yOS0xMTo0NzoxNiAgICAgICAgIj4gPHJkZjpS",
"REYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4g",
"PHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5j",
"b20veGFwLzEuMC9tbS8iIHhtbG5zOnN0UmVmPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5",
"cGUvUmVzb3VyY2VSZWYjIiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHht",
"bG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIgeG1wTU06T3JpZ2luYWxEb2N1",
"bWVudElEPSJ1dWlkOjVEMjA4OTI0OTNCRkRCMTE5MTRBODU5MEQzMTUwOEM4IiB4bXBNTTpEb2N1bWVu",
"dElEPSJ4bXAuZGlkOjY3N0M4Q0ZBMTAzRjExRTg4QTdERDA0QThGRDk0NEZCIiB4bXBNTTpJbnN0YW5j",
"ZUlEPSJ4bXAuaWlkOjY3N0M4Q0Y5MTAzRjExRTg4QTdERDA0QThGRDk0NEZCIiB4bXA6Q3JlYXRvclRv",
"b2w9IkFkb2JlIElsbHVzdHJhdG9yIENDIChNYWNpbnRvc2gpIj4gPHhtcE1NOkRlcml2ZWRGcm9tIHN0",
"UmVmOmluc3RhbmNlSUQ9InhtcC5paWQ6ODY3NGM4MzItMjk2ZC00NDJkLThjNTgtMWQ0NTgyZTliYzI1",
"IiBzdFJlZjpkb2N1bWVudElEPSJ4bXAuZGlkOjg2NzRjODMyLTI5NmQtNDQyZC04YzU4LTFkNDU4MmU5",
"YmMyNSIvPiA8ZGM6dGl0bGU+IDxyZGY6QWx0PiA8cmRmOmxpIHhtbDpsYW5nPSJ4LWRlZmF1bHQiPlBy",
"aW50PC9yZGY6bGk+IDwvcmRmOkFsdD4gPC9kYzp0aXRsZT4gPC9yZGY6RGVzY3JpcHRpb24+IDwvcmRm",
"OlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIiPz4B//79/Pv6+fj39vX08/Lx8O/u7ezr",
"6uno5+bl5OPi4eDf3t3c29rZ2NfW1dTT0tHQz87NzMvKycjHxsXEw8LBwL++vby7urm4t7a1tLOysbCv",
"rq2sq6qpqKempaSjoqGgn56dnJuamZiXlpWUk5KRkI+OjYyLiomIh4aFhIOCgYB/fn18e3p5eHd2dXRz",
"cnFwb25tbGtqaWhnZmVkY2JhYF9eXVxbWllYV1ZVVFNSUVBPTk1MS0pJSEdGRURDQkFAPz49PDs6OTg3",
"NjU0MzIxMC8uLSwrKikoJyYlJCMiISAfHh0cGxoZGBcWFRQTEhEQDw4NDAsKCQgHBgUEAwIBAAAh/hlP",
"cHRpbWl6ZWQgdXNpbmcgZXpnaWYuY29tACH5BAUKAD8ALAAAAACBAKgAAAf/gDSCg4SFhoeIiYqLjI2O",
"j5CRkpOUlZaXmJQLm5wXEB2ZoaKjhg0EJyU3qagZEgGsE6usnqS1toqmsp2EMg6gJZuEuRsfBMG3yKKn",
"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",
"VGop40AlCwQAOw=="};


// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

/// Immagine off Base64
const char offImg[][105] = {"data:image/png;base64,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"
};

/// tempo attuale dall'avvio della scheda in [ms]
unsigned long currentTime = millis();
/// variabile contenente il tempo dall'ultima attuazione
unsigned long previousTime = 0; 
/// Tempo di timeout in millisecondi (esempio: 2000ms = 2s)
const long timeoutTime = 2000;


/// contiene la richiesta HTTP
String header;

// ***************************************************************
//
//                            OGGETTI
//
// ***************************************************************

/// Oggetto Web server in ascolto sulla porta 80
WiFiServer server(80);

// ***************************************************************
//
//                            FUNZIONI
//
// ***************************************************************


void WiFiConn(){
  /**
   * Si connette al WiFi. 
   * 
   * Cerca to connersi alla rete WiFi con ssid @code<ssid>@endcode e con password @code<password>@endcode,
   * finche' non e' connessa riprova a connettersi ogni mezzo secondo.
   * > Con boolDebug = true visualizza IP ottenuto dal DHCP della rete 
  */  
  if (boolDebug){
    Serial.print(F("Mi connetto a "));
    Serial.println(ssid);
  }

  // connettiti alla rete <ssid> con password <password>
  WiFi.begin(ssid, password);

  // finche' non sei connesso, cerca di connetterti
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // aspetta 500 [ms]

    if(boolDebug){
      Serial.print(F("."));
    }
  }

  if (boolDebug){
    Serial.println(F(""));
    Serial.print(F("Connesso al WiFi. IP del nodeMCU/webserver: "));
    Serial.println(WiFi.localIP());
  }
}


void setup() {
  /**
   * Inizializza il pin output come output e livello basso e avvia il web server.
   * 
   * La funzione imposta la modalita' del pin @code<outPin>@endcode come output
   * e come livello logico imposta il livello basso.
   * Infine la funzione avvia il web server.
  */
  
  // Inizializza il pin <outPin> come output
  pinMode(outPin, OUTPUT);

  // Imposta <outPin> a livello basso
  digitalWrite(outPin, LOW);

  if (boolDebug){
    Serial.begin(115200); // inizializza seriale con baudrate 115200
    Serial.print(F("Pin "));
    Serial.print(outPin);
    Serial.print(F(" inizializzato a LOW"));  
  }

  // avvia server web
  server.begin();
}


void loop(){
  /**
   * Si connette al WiFi e gestisce richieste dei web client. 
   * 
   * La funzione controlla se la scheda e' connessa alla rete WiFi,
   * se non e' connessa richiama la funzione WiFiConn() per farlo.
   * 
   * Quando la scheda e' connessa si attende la connessione del client.
   * Quando un client si connette viene letta la richiesta byte per byte
   * riconoscendo la route richiesta dal client:
   * - se la route e' /on viene mandato alto l'outPin
   * - se la route e' /off viene mandato basso l'outPin
   * 
   * Infine viene restituita la risposta: la pagina html
   * con gli stili dei pulsanti e:
   * - se l'output e' on viene visualizzato il pulsante off
   * - se l'output e' off viene visualizzato il pulsante on
   * 
   * Dopo aver risposto al client viene chiusa la connessione.
  */

  // Se scheda si disconnette dal WiFi (o non e' connessa), tenta connessione
  if (WiFi.status() != WL_CONNECTED){
    WiFiConn();     // connettiti al WiFi
    return;         // ricomincia il loop per assicurarsi di essere connessi
  }

  // Resta in ascolto di nuovi client
  WiFiClient client = server.available();

  // Se si e' connesso un client...
  if (client) {
    if (boolDebug){
      Serial.println(F("Nuovo client connesso"));
    }

    // contieni dati provenienti dal client
    String currentLine = "";

    // memorizza il tempo attuale e salvalo come tempo dell'ultima connessione di client
    currentTime = millis();
    previousTime = currentTime;

    // finche' il client e' connesso...
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      // memorizza il tempo attuale
      currentTime = millis();

      // se ci sono dati da leggere dal client...
      if (client.available()) {
        char c = client.read(); // leggi un byte
        header += c;            // e aggiungilo all'header
        
        if (boolDebug){
          Serial.write(c);
        }

        // controlla cosa contiene il byte
        if (c == '\n') {
          // se la riga e' vuota, la richiesta del client e' terminata, rispondi:
          if (currentLine.length() == 0) {
            // l'header HTTP inizia con un codice di stato (esempio HTTP/1.1 200 OK)
            // e il tipo di contenuto e infine una riga vuota:
            client.println("HTTP/1.1 200 OK");         // codice OK
            client.println("Content-type:text/html");  // restituisco HTML
            client.println("Connection: close");
            client.println();                          // divido contenuto dall'header
            
            // controlla la route richiesta per impostare lo stato dell'output
            if (header.indexOf("GET /on") >= 0) {
              // richiesta su "/on", mando alto il pin
              if (boolDebug){
                Serial.print(F("Richiesta su /on, mando alto il pin "));
                Serial.println(outPin);
              }
              
              outState = "on";            // memorizza che il nuovo stato del pin e' on
              digitalWrite(outPin, HIGH); // manda alto il pin
            } 
            else if (header.indexOf("GET /off") >= 0) {
              // richiesta su "/off", mando basso il pin
              if (boolDebug){
                Serial.print(F("Richiesta su /off, mando basso il pin "));
                Serial.println(outPin);
              }

              outState = "off";          // memorizza che il nuovo stato del pin e' off
              digitalWrite(outPin, LOW); // manda basso il pin
            }
            
            // Restituisci la pagina HTML
            client.println("<!DOCTYPE html><html>"); // usa HTML5 e apri tag principale
            // nella head aggiungi come metadata il viewport responsive (la pagina si adatta allo schermo del dispositivo)
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            // impedisci richiesta di favicon (l'icona della pagina)
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // -- CSS
            // apri il tag del CSS
            client.println("<style>");

            // aggiungi stile html (font, allineamento al centro,...)
            client.println(htmlCSS);
            
            // aggiungi stile pulsanti
            client.println(btnCSS);
            // aggiungi stile pulsante off
            client.println(offBtnCSS);
            // aggiungi stile pulsante on
            client.println(onBtnCSS);

            // chiudi tag del CSS e head
            client.println("</style></head>");
            
            // -- body
            // visualizza un titolo
            client.println("<body><h1>WEBSERVER FAN</h1><h2>CONTROLLER</h2>");
            
            // Visualizza stato dell'output e il pulsante con lo stile corretto  
            client.println("<p>Stato output: " + outState + "</p>");
            
            if (outState == "off") {
              // se l'output e' off visualizza il pulsante azzurro con la scritta "ON".
              // il pulsante e' contenuto in un anchor tag (collegamento) che fa richiesta
              // al server web stesso sulla pagina "/on" per accendere l'output
              client.print("<img src=\"");

              // restituisci al client porzioni di base64 dell'immagine (non e' possibile inviarlo tutto in una volta sola)
              for (int i=0; i<dim(offImg); i++){
                client.print(offImg[i]);
              }
              
              client.println("\">");
              client.println("<p><a href=\"/on\"><button class=\"button on\">ON</button></a></p>");
            } 
            else {
              // se l'output e' on visualizza il pulsante grigio con la scritta "OFF".
              // il pulsante e' contenuto in un anchor tag (collegamento) che fa richiesta
              // al server web stesso sulla pagina "/off" per spegnere l'output
              client.print("<img src=\"");

              // restituisci al client porzioni di base64 dell'immagine (non e' possibile inviarlo tutto in una volta sola)
              for (int i=0; i<dim(onImg); i++){
                client.print(onImg[i]);
              }

              client.println("\">");
              client.println("<p><a href=\"/off\"><button class=\"button off\">OFF</button></a></p>");
            } 
            
            // chiudi il body e il tag html
            client.println("</body></html>");
            
            // Termina la risposta HTTP con una riga vuota
            client.println();
            // esci dal loop
            break;
          } 
          else { 
            // se ottieni un newline, resetta currentLine
            currentLine = "";
          }
        } 
        else if (c != '\r') {  
          // se e' arrivato qualcosa di diverso dal carattere return
          currentLine += c; // aggiungilo alla fine di currentLine
        }
      }
    }
    
    // Resetta la variabile header
    header = "";
    // Chiudi la connessione
    client.stop();

    if (boolDebug){
      Serial.println(F("Client disconnesso."));
      Serial.println(F(""));
    }
  }
}
