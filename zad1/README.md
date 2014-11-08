ZADANIE NR 1

Należy napisać w języku C program Pascal implementujący jednowymiarową tablicę n+1 (n > 0) procesów generujących n-ty wiersz trójkąta Pascala (por. np. http://pl.wikipedia.org/wiki/Tr%C3%B3jk%C4%85t_Pascala; w treści zadania zakładamy, że wiersze trójkata Pascala są ponumerowane od 1). Tablica powinna zawierać jeden proces Pascal oraz n procesów W. Docelowo każdy z procesów W(i) dokonuje wyliczenia i-tego współczynnika n-tego wiersza trójkąta.

DZIAŁANIE I KOMUNIKACJA

Program Pascal wywoływany jest z jednym parametrem n (n >0) określającym, który wiersz trójkąta Pascala ma zostać wyliczony, po czym tworzona jest lista n procesów W.

Proces Pascal komunikuje się wyłącznie z procesem W(1). Każdy z procesów W(i), z wyjątkiem pierwszego i ostatniego, komunikuje się z procesami W(i-1) i W(i+1). Proces W(1) komunikuje się z procesem Pascal oraz procesem W(2) a proces W(n) z W(n-1).

Obliczenia odbywają się w n krokach. Każdy i-ty krok obliczeń inicjowany jest przez proces Pascal i obejmuje i kolejnych procesów: W(1), W(2), ... W(i). Wyliczenie nowej wartości współczynnika k (dla k>1 i k<i) polega na dodaniu do starej (czyli wyliczonej w poprzednim kroku) wartości tego współczynnika , starej wartości współczynnika procesu o numerze k-1. Współczynniki o numerach 1 i k mają zawsze wartość 1.

Po wykonaniu wszystkich kroków obliczeń współczynniki przekazywane są w odpowiedniej kolejności do procesu Pascal, który wypisuje je na standardowym wyjściu (można skorzystać z tego, że wiersze trójkąta Pascala są symetryczne). Po zakończeniu obliczeń i przekazaniu wszystkich koniecznych informacji, procesy W kończą pracę. Proces Pascal kończy pracę po wypisaniu wyników.

Do komunikacji miedzy procesami wykorzystywane są wyłącznie łącza nienazwane. Komunikacja odbywa się dwukierunkowo.

PROTOKÓŁ KOMUNIKACYJNY

Ustalenie szczegółów użytego protokołu komunikacyjnego należy do autora implementacji. Jego specyfikacja powinna zostać podana w oddzielnym pliku tekstowym.

POZOSTAŁE INFORMACJE

Rozwiązania (tylko Makefile, pliki źródłowe i plik z opisem protokołu opis.txt) należy nadsyłać za pomoca skryptu submit na adres: solab@mimuw.edu.pl

W komentarzu w kodzie źródłowym nalezy umieścić dane autora (nazwisko, numer albumu).

Kod źródłowy powinien być w rozsądny sposób skomentowany.

Wszystkie procesy są zobowiązane do oczekiwania na zakończenie swoich potomków.

Należy bezwzględnie zamykać nieużywane deskryptory plików.

Należy sprawdzać poprawność wykonania funkcji systemowych.

Ostateczny termin nadsyłania rozwiązań to: 17 listopada 2014, godz. 23:59.

W przypadku wątpliwości można zadać pytanie autorowi zadania K. Szafranowi. Przed zadaniem pytania warto sprawdzić, czy odpowiedź na nie nie pojawiła się na liście często zadawanych pytań.
