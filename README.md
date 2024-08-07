# Cache-Simulation

## Typische Größen von Caches

Bei einen intel core i7, hat der L1- und L2-Cache, jeweils 32KB und 256KB. Der größte Cache, L3, wird über alle CPU-Kerne geteilt und hat eine Kapazität von 8MB. Da L1 Caches typischerweise Direct Mapped sind, und L3 8 fach assoziativ oder Vollassoziativ sind, stellen wir fest, dass in CPUs, schnelle, Direct Mapped Caches eine Tendenz haben, kleiner zu sein.

## Zugriffzeiten
Von der quelle von Norvig.com und von unserer eigenen Computer, bestimmen wir estimierte Zugriffszeiten:

| Cache-Typ                | Zugriffzeit |
|--------------------------|-------------|
| L1-Cache-Zugriff         | ~0,5 ns      |
| L2-Cache-Zugriff         | ~3 ns        |
| L3-Cache-Zugriff         | ~11 ns        |
| Hauptspeicher-Zugriff    | ~80 ns      |

Um speicherintensive Algorithmen zu analysieren, benutzen wir der Matrixmultiplikations-Algorithmus. Gewählt wegen einer Komplexität von O(n^3) hohe Anzahl Speicherzugriffe, dies ermöglicht, umfassende Benchmarks unserer Simulation zu machen.
Wir haben ein einfaches Programm in C geschrieben, das eine CSV-Datei für einen Matrixmultiplikationsalgorithmus generiert. Jeder Speicherzugriff wird geloggt, um es zu ermöglichen Benchmarks zu simulieren.

Bevor wir mit der CSV-Datei Benchmarks durchführen, beobachten wir zunächst die Speicherzugriffsmuster in der CSV-Datei. Ein klares Muster von "WRRWRRWRRWRRWRRWRRWRRW", das sich durch die gesamte CSV-Datei wiederholt, ist erkennbar. Sieben Doppel-Lesezugriffe, die von Schreibzugriffen umgeben sind. Ein ähnliches Muster beobachten wir bei der Analyse einer 2x2-Matrix, "WRRWRRW". Ein klares Speicherzugriffsmuster. Wir vermuten, dass dies bei vielen speicherintensiven Algorithmen der Fall ist, bei denen viel vorhersehbare Arbeit geleistet wird. Wir schlagen vor, dass dies das Konzept ist, auf dem moderne Cache-Technologien wie das Prefetching basieren.

## Cache-Treffer und -Fehlzugriffe

| Cache-Typ           | 8 Cachezeilen / 4 Byte | 16 Cachezeilen / 8 Byte | 32 Cachezeilen / 16 Byte |
|---------------------|------------------------|-------------------------|--------------------------|
| Direct mapped       | 526/552                | 869/209                 | 1029/49                  |
| Vollassoziativ      | 426/652                | 846/232                 | 990/88                   |

Hier ist die erste Zahl immer die Anzahl der Hits, die zweite die Anzahl von Misses. Wenn ein Cache-Zugriff 2 Zyklen dauert und ein Speicherzugriff 100 Zyklen, dann ist die Zyklenzahl jeweils:

## Zyklenzahl

| Cache-Typ           | 8 Cachezeilen / 4 Byte | 16 Cachezeilen / 8 Byte | 32 Cachezeilen / 16 Byte |
|---------------------|------------------------|-------------------------|--------------------------|
| Direct mapped       | 56252                  | 22638                   | 6958                     |
| Fully associative   | 66052                  | 24892                   | 10780                    |

Und wo die Zyklenzahl ohne Cache 107800 (100% Miss rate) wäre. 
Dies bedeutet, dass wir eine Beschleunigung von bis zu 15x beobachten! (Wir stellen auch fest, dass unser Direct Mapped Cache besser abschneidet als unser fully associative Cache, was nicht der Fall sein sollte.)

## Arbeitsleistung

**Linus:**

**Yannick:** 
Ich habe die java prototypen erstellt um die programstruktur und die implementiereung fest zu stellen, damit wir unser c/c++ projekt nach was modellieren könnten, damit auch frühe drafts von die cache algorithmen bevor Linus übernommen hat. Ich habe auch den theoretischen teil gemacht, den README erstellt und die presenatation gemacht, wozu ich auch unser ergebnis daten verarbeitet habe. Ich habe Auch file_processing.cpp/c und data_gen.cpp/c erstellt.

**Jonas:** “main.c”, “file_processing.c” und “matrix_multiplication.c”

(Quellen sind unter der "Sources" slide, in der Präsentation(slides/slides.pdf))
