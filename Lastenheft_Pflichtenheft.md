# Ziel:
Entwicklung eines Operationsplanungssystems zur Versorgung eines Oberschenkelhalsbruches mit Hilfe einer dynamischen Hüftschraube


# Lastenheft
## Technische Anforderungen:
* zur Planung einer Bohr-Trajektorie sollen die Start- und Endpunkte vom Nutzer auswählbar sein.
* Nach dem Auswahl der Start- und Endpunkte soll der Abstand zwischen diesen beiden Punkten ausgemessen und ausgegeben werden.
* Der Bohrdurchmesser soll vom Nutzer auswählbar sein.
Entlang der Trajektorie orthogonal zur Borrichtung sollen die Bild-Schichten einzeln dargestellt werden. Über ein Slider ist die Schicht auszuwählen.

## Allgemeine Anforderungen:
* Das Programm soll Modular aufgebaut sein, sodass eine Erweiterung und Wiederverwendung in anderen Programen möglich ist.
* Die Entwickungsprozess soll dokumentiert sein. Hierzu kann eine Versionsverwaltung der Nachvollziehbarkeit des Entwicklungsprozesses dienen 
* User Interface soll benutzerfreundlich sein.
* Der Anwender soll zur Laufzeit über potentielle Risiken / Fehler informiert werden.


** LASTENHEFT SOLL NOCH ERGÄNZT WERDEN. **

# Pflichtenheft: Wie sollen die Anforderungen realisiert werden?
## Anforderungen an das GUI:
* es soll eine Interaktive GUI entwickelt werden, die dem Nutzer die Möglichkeit gibt, einen Star- und einen Endpunkt über die Mauts zu wählen.
* Außerdem kann der Anwender über ein interaktives GUI über den aktuellen Status des Programms informiert werden.

## Anforderungen an die DLL: 
* Es soll eine Funktion entwickelt werden, welche es ermöglich, ein CT-Datei zu laden und speichern.
* Es soll eine Funktion entwickelt werden, welche es ermöglich, eine gewählte Schicht der gespeicherten CT-Datei zu visualisieren.

* Es soll eine Windowing-Funktion entwickelt werden, welche die in einem bestimmten Fenster liegendeF Grauwerte darstellt und alle anderen Grauwerte ausfiltert.

## Allgemeine Implementierunganforderungen:
* Das Programm soll in C++ implementiert werden.

** PFLICHTENHEFT SOLL NOCH ERGÄNZT WERDEN. **
