
#Aleksandra Polek 204150 grupa 4a 
# Projekt SCADA 
## Opis Projektu 
Aplikacja jest symulatorem systemu SCADA napisanym w języku Python przy użyciu biblioteki graficznej PyQt5.  
Program wizualizuje proces przepływu cieczy przez kaskadę 4 zbiorników, symuluje zjawiska fizyczne (zmiana temperatury, wrzenie, wzrost ciśnienia) oraz obsługuje stany awaryjne (eksplozja). 
## Funkcjonalność 
* Wizualizacja: Graficzna reprezentacja 4 zbiorników (Z1-Z4), rur łączących oraz poziomu cieczy. 
* Animacja przepływu: Rury zmieniają kolor na niebieski w momencie aktywnego przepływu cieczy. 
* Symulacja fizyki: 
    * Mieszanie cieczy o różnych temperaturach. 
    * Ogrzewanie wody w zbiorniku końcowym (Z4 - Parownik). 
    * Symulacja wrzenia wody (przy 100°C) i wzrostu ciśnienia. 
    * Naturalne stygnięcie do temperatury otoczenia (20°C). 
* Wizualizacja parametrów: 
    * Kolor wody zmienia się płynnie od niebieskiego (zimna) do czerwonego (gorąca).
    * Wskaźnik ciśnienia dla parownika. 
* System Alarmowy: Wyświetlanie okna w momencie krytycznej awarii (wybuchu zbiornika). 
* Interakcja: Użytkownik może dolewać ciecz, sterować zaworami oraz włączać grzałkę. 
