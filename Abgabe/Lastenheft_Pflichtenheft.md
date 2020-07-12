# Ziel:
Entwicklung eines Operationsplanungssystems zur Versorgung eines Oberschenkelhalsbruches mit Hilfe einer dynamischen Hüftschraube


# Lastenheft
## Technische Anforderungen:
* zur Planung einer Bohr-Trajektorie sollen die Start- und Endpunkte vom Nutzer auswählbar sein.
* Nach der Auswahl der Start- und Endpunkte soll der Abstand zwischen diesen beiden Punkten ausgemessen und ausgegeben werden.
* Der Bohrdurchmesser soll vom Nutzer auswählbar sein.
* Entlang der Trajektorie orthogonal zur Bohrrichtung sollen die Bild-Schichten einzeln dargestellt werden. Über ein Slider ist die Schicht auszuwählen.
* Das 3D-Bild soll in mindestens eine Ansicht dargestellt werden.
* Die rekonstruierte Schicht soll in einer separaten Ansicht dargestellt werden.
* Die geplante Bohrung soll in dem rekonstruierten Bild dargestellt werden, sodass eine visuelle Prüfung entlang des einzubohrenden Knochens möglich ist.

## Allgemeine Anforderungen:
* Das Programm soll modular aufgebaut sein, sodass eine Erweiterung und Wiederverwendung in anderen Programen möglich sind.
* Der Entwicklungsprozess soll dokumentiert sein. Hierzu kann eine Versionsverwaltung der Nachvollziehbarkeit des Entwicklungsprozesses dienen 
* User Interface soll möglichst benutzerfreundlich sein.
* Der Anwender soll zur Laufzeit über potenzielle Risiken / Fehler informiert werden.


# Pflichtenheft: Wie sollen die Anforderungen realisiert werden?
## Anforderungen an das GUI:
* Es soll eine Interaktive GUI entwickelt werden, die dem Nutzer die Möglichkeit gibt, einen Star- und einen Endpunkt über die Maustaste zu wählen.
* Außerdem soll der Anwender über ein interaktives GUI über den aktuellen Status des Programms informiert werden.
* Eine intuitive Gestaltung und eine kurze Anleitung sollen die Benutzerfreundlichkeit und die richtige Anwendung versichern.

## Anforderungen an die DLL: 
* Es soll eine Funktion entwickelt werden, welche es ermöglich, eine CT-Datei zu laden und zu speichern.
* Es soll eine Funktion entwickelt werden, welche es ermöglich, eine gewählte Schicht der gespeicherten CT-Datei zu rekonstruieren.
* Es soll eine Windowing-Funktion entwickelt werden, welche die in einem bestimmten Fenster liegende Grauwerte linear interpoliert und alle anderen Grauwerte ausfiltert.
* weitere wiederverwendbare Funktionen sollen nach Möglichkeit in der DLL ausgelagert werden.

## Allgemeine Implementierung-Anforderungen:
* Das Programm soll in C++ implementiert werden.

