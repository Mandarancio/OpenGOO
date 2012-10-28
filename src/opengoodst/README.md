OpengooDebugSenderTool - OpenGooDst {#mainpage}
======

 * [English](#english)
 * [Italiano](#italiano)

# English

 * [Preface](#preface)
 * [How to test](#how-to-test)
 * [Contribute](#how-to-contribute)

##Preface

OpenGooDst is an OpenGoo module that allows users to send a bug report when the game crashes. The report will be filled with the backtrace + debug output text and a small description from the user about his steps.

OpenGooDst is indipendent from OpenGoo and is composed by a wizard and a little manager that shows all the reports, the sent and not sent. The user can also use the manager to send the not sent reports or to have feedback on the sent reports ([resolution feedback](#resolution-feedback))

OpenGoo, when a crash occurs, stores the backtrace and the debug output in a file [debug.xml](#the-xml-file-structure) and generates a unique identifier "Uuid" related to the report. Further it runs OpenGooDst passing the generated Uuid via arguments.

The OpenGoo module which opens the debug.xml file and stores the report is called on the branch "XML-Module". When finished it will be merged with the game code.

OpenGooDst is localized on startup with the language set in the game config file.

How to detect the config file path, like a lot of other information, is delegated to the module which will install OpenGoo on the system.

We need to design how to gather this information on different architectures.

###The startup:

On startup OpenGooDst checks the arguments to detect the wanted mode:

	-w -uuid=UUID_REPORT for the wizard mode.
	-m for the manager mode.

In wizard mode, OpenGooDst opens the debug.xml file and searches the report with the Uuid passed, then it fills the report with the information set by the user and sends it.

###The xml file structure:


    <!DOCTYPE debugFile>
    <gameReports>

      <sysInfo os="*" architecture="*" build="*" gameVer="*" vga="*" driverVer="*" inUse="?">
    
          <report uuid="*" date-time="*" debugOutput="*" userDescription="*" sent="?" sendDate="*" fixed="?" gameVerFix="*"/>

    	 <report uuid=... />

               [...]

      </sysInfo>

      <sysInfo os="*"  ...

           <report uuid=...

      </sysInfo>

    </gameReports>

Hierarchically there are two nodes: the architecture and the report.

The Architecture: contains all the information related to the architecture like the machine where OpenGoo runs and few information about the game (version number, build number, vga type, etc...).

The Report: is a single bug report containing all the information about, like the backtrace and the debug text, the user description, the dateTime when it happened and overhead information.
The report is hierarchically related to the architecture where it is contained.

The debug.xml file is populated when a crash occured detecting the architecture in use. When the game is updated to a new version a new architecture is appended and the new reports are tagged along this one.

Before delve into each field we clarify that we want to implement the opportunity of provide [feedback](#resolution-feedback) to the user when a sent bug has been fixed. Additional fields are added for this purpose, like the version of the game where the bug is fixed.

###The fields:

Architecture:
      
    <sysInfo os="*" arch="*" build="*" gameVer="120" vga="*" driverVer="*" inUse="true">

    </sysInfo>

 * os = operating system in use.
 * arch = numeric id of the architecture.
 * build = numeric build of the game.
 * gameVer = game version.
 * vga = gpu model.
 * driverVer = driver version of the gpu.
 * inUse = flag that indicates the architecture in actual use.

Report:

    <report uuid="*" date-time="*" debugOutput="**--#dump" userDescription="Has crashed!" sent="false" sendDate="*" fixed="true" gameVerFix="*"/>

 * uuid = unique identifier of the report.
 * date-time = date/time when the crash occurs in UTC.
 * debugOutput = backtrace and debug output.
 * userDescription = user's textual description.
 * sent = flag that indicates if the report is sent.
 * sendDate = date/time of the sent report.
 * fixed = flag that indicates if the report has been fixed.
 * gameVerFix = the game version where the report has been fixed.

###The textual report:

The report format, sent by OpenGooDst, is generated from the information allocated:

    OpenGooDTS Report uuid generated on dateTime
    -----------------------------------------------------
    -                    Architecture                   -
    -----------------------------------------------------
    - GameVer in Build
    - Os on Arch with Vga, DriverVer
    -----------------------------------------------------
    -                     Report                        -
    -----------------------------------------------------
    -Tracer=
    DebugText
    -UserDescription=
    UserText
    -----------------------------------------------------
    Sent on sendDate

(Could change deeply)

The report will be uploaded via ftp protocol. Actually we have no server for this purpose.

### Resolution feedback:
(Actually not implemented)  
The report is generated each time with a universal-unique identifier or with few chances of replica.
When a bug has been fixed, an xml file containing the report uuid + additional information is updated on the server.

During the startup in manager mode, OpenGooDst downloads the xml file searching feedback news.

Therein lies the problem of handling in an efficient way the reports. It is possible to design a different kind of textual reports.

##How to test:

The qt libraries are required.

You need an ftp server where upload the reports.
Edit wizardgui_s2.cpp at line 62 and insert the host and the credentials:

    ftp->connectToHost("HOST");
    ftp->login("USERNAME", "PASSWORD");

then compile following these statements.

 * qmake
 * make
 
 * ./OpenGooDst -m

Make sure the text.xml file is in the same directory of the binary and the program is launched with the right arguments.

##How to contribute:

Soon different issues will be opened on github and you can take one or more of these tasks.
This is a short list of todo tasks:

 * Ftp upload code.
 * qtLinguistic code.
 * design of feedback file structure and related code.
 * Xml-Module code.
 * Two Goos artworks.

======

Initial Idea and Design:  
Fabrizio Signoretti - fasigno37@gmail.com  
Under GPLv3  

***

# Italiano

ATTENZIONE, La lingua italiana non verrà più aggiornata usare la versione inglese come riferimento.

 * [Introduzione](#introduzione)
 * [Compilazione](#compilazione)
 * [Contribuire](#come-contribuire)

##Introduzione 
OpenGooDst è un modulo di OpenGoo che permette di inviare al momento del crash del gioco un report formato dal backtrace + debug output e una breve descrizione dell'utente su quanto svolgeva al momento del crash.

OpenGooDst è separato da OpenGoo ed è formato da un wizard di invio report e un piccolo manager, in grado di visualizzare tutti i report, inviati e non inviati. E' inoltre possibile inviare dal manager eventuali report non inviati e di avere un [feedback di risoluzione](#feedback-di-risoluzione) di un particolare bug.

OpenGoo, al momento del crash, memorizza il backtrace e il debug output nel file [debug.xml](#la-struttura-del-file-xml), generando un identificatore univoco “Uuid” legato al report. Successivamente esegue OpenGooDst passandogli l'uuid come argomento.

Il modulo di OpenGoo che apre il file di debug.xml e memorizza il report è sul branch denominato: “XML-Module”. Non appena concluso sarà inglobato nel gioco.

OpenGooDst viene localizzato all'avvio con la lingua impostata nel gioco, tra quelle che saranno disponibili.

La determinazione del percorso al file di configurazione del gioco, come molte altre informazioni di architettura, è demandata al modulo che installerà OpenGoo nel sistema.

Vi è il problema di come raccogliere queste informazioni sulle diverse architetture su cui OpenGoo sarà distribuito.

###L'avvio:

All'avvio OpenGooDst valuta i parametri per determinare in che modalità avviarsi:

	-w -uuid=UUID_REPORT per wizard
	-m per manager

In avvio in modalità wizard, OpenGooDst apre il file debug.xml in cerca del report identificato dall'uuid passato come argomento, lo completa delle informazioni inserite dall'utente e lo invia.

###La struttura del file xml:


    <!DOCTYPE debugFile>
    <gameReports>

      <sysInfo os="*" architecture="*" build="*" gameVer="*" vga="*" driverVer="*" inUse="?">
    
          <report uuid="*" date-time="*" debugOutput="*" userDescription="*" sent="?" sendDate="*" fixed="?" gameVerFix="*"/>

    	 <report uuid=... />

               [...]

      </sysInfo>

      <sysInfo os="*"  ...

           <report uuid=...

      </sysInfo>

    </gameReports>

Gerarchicamente vi sono due nodi: l'architettura e il report.

L'Architettura: contiene tutte le informazioni strettamente architetturali, relative alla macchina su cui è installato i gioco e relative al gioco stesso (versione, num di build, vga, ecc...)

Il Report: è una singola segnalazione di un avvenuto bug, contenente tutte le informazioni a riguardo come il backtrace e il debug, la descrizione dell'utente, l'ora di avvenimento e informazioni di supporto.
Il report è legato gerarchicamente all'architettura che lo contiene.

Il file debug.xml viene popolato al momento del crash identificando l'architettura attualmente in uso. Quando il gioco viene aggiornato di versione, viene aggiunta una nuova architettura e i report verranno accodati a questa.

Prima di entrare nel dettaglio dei singoli campi del file debug.xml è bene precisare che un obbiettivo che vogliamo raggiungere è la possibilità di segnalare all'utente che quel bug segnalatoci è stato risolto, indicando inoltre con quale versione del gioco. A questo proposito alcuni campi sono progettati a supporto di [questa funzionalità](#feedback-di-risoluzione).

###I campi:

Architettura:
      
    <sysInfo os="*" arch="*" build="*" gameVer="120" vga="*" driverVer="*" inUse="true">

    </sysInfo>

 * os = sistema operativo in uso.
 * arch=id numerico dell'architettura.
 * build=numero di build del gioco.
 * gameVer=versione del gioco.
 * vga=modello di scheda video.
 * driverVer=versione del driver della scheda video.
 * inUse=flag che indica se l'architettura è quella attualmente in uso.

Report:

    <report uuid="*" date-time="*" debugOutput="**--#dump" userDescription="Has crashed!" sent="false" sendDate="*" fixed="true" gameVerFix="*"/>

 * uuid= identificativo univoco del report.
 * date-time= data/ora di avvenuto crash in UTC.
 * debugOutput= testo di backtrace + debug.
 * userDescription= descrizione a parole dell'utente.
 * sent = flag che indica se il report è stato inviato in rete.
 * sendDate = data/ora di invio in rete del report.
 * fixed= indica se il bug segnalato è stato risolto.
 * gameVerFix= indica la versione di gioco con cui è stato risolto il bug segnalato.

###Il report testuale:

Il formato del report inviato è generato da OpenGooDst, sulla base delle informazioni allocate, secondo il seguente formato:

    OpenGooDTS Report uuid generated on dateTime
    -----------------------------------------------------
    -                    Architecture                   -
    -----------------------------------------------------
    - GameVer in Build
    - Os on Arch with Vga, DriverVer
    -----------------------------------------------------
    -                     Report                        -
    -----------------------------------------------------
    -Tracer=
    DebugText
    -UserDescription=
    UserText
    -----------------------------------------------------
    Sent on sendDate

(Potrà subire sostanziali modifiche)

Il report verrà uplodato via ftp. Attualmente non abbiamo un server per lo scopo.

###Feedback di risoluzione:
(Funzionalità attualmente non implementata)  
Il report viene generato ogni volta con un uuid univoco universalmente o con poche probabilità di duplicato.
Quando un bug viene risolto, viene aggiornato sul server un file xml contenente l'uuid del report + informazioni supplementari, come la versione del gioco con cui il bug è risolto.

All'avvio del manager, OpenGooDst scarica il file .xml cercando eventuali feedback.

Sorge il problema di gestire in modo efficiente i reports. È possibile quindi progettare un diverso tipo di file di segnalazione, raggruppabile con altri e con un formato più manipolabile.  

##Compilazione:

Per compilare sono necessarie le librerie qt.  
Svolgere i seguenti comandi.

 * qmake
 * make
 
 * ./OpenGooDst

Assicurarsi che il file dimostrativo test.xml sia nella cartella insieme all'eseguibile e che il programma venga eseguito con gli opportuni argomenti.

##Come contribuire:

Diversi issues verranno aperti su github e potrai assegnarti ad alcuni di essi.
Questa è una breve lista del lavoro da svolgere:

 * Ftp upload code.
 * qtLinguistic code.
 * design of feedback file structure and related code.
 * Xml-Module code.
 * Two Goos artworks.

======

Initial Idea and Design:  
Fabrizio Signoretti - fasigno37@gmail.com  
Under GPLv3  
