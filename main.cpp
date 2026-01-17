import sys
import random
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QDialog, QVBoxLayout
from PyQt5.QtCore import Qt, QTimer, QPointF, QRectF
from PyQt5.QtGui import QPainter, QColor, QPen, QPainterPath, QFont

#ustawianie stałych początkowych wartoci
TEMP_WRZENIA = 100.0
TEMP_OTOCZENIA = 20.0
CISNIENIE_MAX = 100.0

SZYBKOSC_GRZANIA = 1.2
SZYBKOSC_PAROWANIA = 0.5
PREDKOSC_PRZEPLYWU = 2.0

class OknoAwarii(QDialog):
    def __init__(self, parent=None): #tworzenie okna, wymiary itp
        super().__init__(parent)
        self.setWindowTitle("AWARIA")
        self.setFixedSize(300, 150)
        self.setStyleSheet("background-color: black;")
        layout = QVBoxLayout()
        tekst = QLabel("AWARIA", self)
        tekst.setAlignment(Qt.AlignCenter)
        tekst.setFont(QFont("Arial", 28, QFont.Bold))
        tekst.setStyleSheet("color: red;")
        layout.addWidget(tekst)
        
        przycisk = QPushButton("NAPRAW", self) #tworzenie przycisku
        przycisk.setMinimumHeight(40)
        przycisk.setStyleSheet("background-color: blue; color: white; font-weight: bold; border: none;")
        przycisk.clicked.connect(self.accept) #akceptowanie
        layout.addWidget(przycisk)
        self.setLayout(layout)

class Rura:
    def __init__(self, punkty_trasy):
        self.punkty = [QPointF(float(p[0]), float(p[1])) for p in punkty_trasy] #tworzenie trasy rury
        self.czy_plynie = False #czy plynie

    def ustaw_przeplyw(self, stan):
        self.czy_plynie = stan

    def draw(self, painter):
        if len(self.punkty) < 2: return # czy da sie narysowac
        sciezka = QPainterPath()
        sciezka.moveTo(self.punkty[0]) #start
        for p in self.punkty[1:]: sciezka.lineTo(p) #rysowanie do kolejnych punktow
        painter.setPen(QPen(QColor(60, 60, 60), 12, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin)) #wyglad
        painter.setBrush(Qt.NoBrush)
        painter.drawPath(sciezka)

        if self.czy_plynie: #jak plynie inny widok rury
            painter.setPen(QPen(QColor(0, 120, 255), 8, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin))
            painter.drawPath(sciezka)

class Zbiornik:
    def __init__(self, x, y, w, h, nazwa):
        self.rect = QRectF(x, y, w, h) #pamietanie pozycji i poczatkowe wartosci
        self.nazwa = nazwa
        self.pojemnosc = 100.0
        self.ilosc = 0.0
        self.temperatura = 20.0
        self.cisnienie = 0.0
        self.grzalka_on = False
        self.zniszczony = False
        self.zawor_otwarty = True 

    def oblicz_fizyke(self):
        if self.zniszczony: return #sprawdza czy zepsuty
        
        # Temperatura
        if self.grzalka_on: #dzialanie grzalki
            self.temperatura += SZYBKOSC_GRZANIA
        else: 
            self.temperatura -= (self.temperatura - TEMP_OTOCZENIA) * 0.02

        if self.temperatura > 100.0: #maks wartosc
            self.temperatura = 100.0
        
        if self.temperatura < TEMP_OTOCZENIA: 
            self.temperatura = TEMP_OTOCZENIA

        czy_wrze = (self.temperatura >= TEMP_WRZENIA and self.ilosc > 0) #woda wrze
        
        if czy_wrze:
            self.cisnienie += 1.5 if self.grzalka_on else 0.5 #cisnienie rosnie i para, ubywa wody
            self.ilosc -= SZYBKOSC_PAROWANIA
        else:
            if self.cisnienie > 0: self.cisnienie -= 0.5

        if self.cisnienie < 0: self.cisnienie = 0
        if self.ilosc < 0: self.ilosc = 0

        if self.cisnienie >= CISNIENIE_MAX: #wybuch 
            self.zniszczony = True; self.ilosc = 0; self.cisnienie = 0; self.temperatura = 20 #zerowanie wartosci

    def napraw(self):
        self.zniszczony = False; self.cisnienie = 0; self.ilosc = 0; self.temperatura = 20
        self.grzalka_on = False

    def dodaj_ciecz(self, ile, temp_wlewanej=20.0): #ile wody wlewamy i temperatura jej
        if self.zniszczony: return 0 #jesli zniszczony nie dolewamy 
        
        wolne = self.pojemnosc - self.ilosc
        wlewamy = min(ile, wolne)
        if wlewamy > 0:
            self.temperatura = (self.ilosc * self.temperatura + wlewamy * temp_wlewanej) / (self.ilosc + wlewamy)
            self.ilosc += wlewamy
        return wlewamy

    def usun_ciecz(self, ile):
        if self.zniszczony: return 0
        
        wylewamy = min(ile, self.ilosc)
        self.ilosc -= wylewamy
        return wylewamy

    def get_kolor_wody(self):
        t = max(0, min(self.temperatura, 100))
        ratio = t / 100.0
        return QColor(int(ratio * 255), int(120 - ratio * 120), int(255 - ratio * 255), 200)

    def draw(self, painter):
        if self.zniszczony: #zniszczony zbiornik rysowanie
            painter.setPen(QPen(Qt.red, 3)); painter.setBrush(QColor(30, 0, 0))
            painter.drawRect(self.rect)
            painter.drawLine(self.rect.topLeft(), self.rect.bottomRight())
            painter.drawLine(self.rect.topRight(), self.rect.bottomLeft())
            return

        if self.ilosc > 0:
            h = self.rect.height() * (self.ilosc / self.pojemnosc) #obliczanie wysokosci wody
            painter.setPen(Qt.NoPen); painter.setBrush(self.get_kolor_wody())
            painter.drawRect(QRectF(self.rect.x()+2, self.rect.bottom()-h, self.rect.width()-4, h-2))

        kolor = Qt.white
        painter.setPen(QPen(kolor, 3)); painter.setBrush(Qt.NoBrush)
        painter.drawRect(self.rect)

        if self.grzalka_on:
            painter.setPen(QPen(Qt.red, 4))
            painter.drawLine(int(self.rect.left()+5), int(self.rect.bottom()-5), int(self.rect.right()-5), int(self.rect.bottom()-5))

        if "Parownik" in self.nazwa: #rysuj pasek cisnienia jesli parownik
            self._rysuj_pasek(painter)

        painter.setPen(Qt.white); painter.setFont(QFont("Arial", 10, QFont.Bold))
        painter.drawText(QPointF(self.rect.left(), self.rect.top() - 10), self.nazwa)
        info = f"{self.temperatura:.1f}°C"
        painter.setFont(QFont("Consolas", 9))
        painter.drawText(QPointF(self.rect.left() + 5, self.rect.top() + 18), info)

    def _rysuj_pasek(self, painter):
        x, y, h = int(self.rect.right() + 10), int(self.rect.top()), int(self.rect.height())
        painter.setPen(Qt.gray); painter.setBrush(Qt.black); painter.drawRect(x, y, 10, h)
        fill = h * (self.cisnienie / CISNIENIE_MAX)
        kolor = Qt.green
        if self.cisnienie > 50: kolor = Qt.yellow
        if self.cisnienie > 80: kolor = Qt.red
        painter.setBrush(kolor)
        painter.drawRect(x, int(y + h - fill), 10, int(fill))
        painter.setPen(Qt.white); painter.setFont(QFont("Arial", 7))
        painter.drawText(x, y - 5, "BAR")

class SystemSCADA(QWidget): 
    def __init__(self): #uruchamia aplikacje
        super().__init__()
        self.setWindowTitle("projekt")
        self.resize(800, 600) 
        self.setStyleSheet("background-color: #222; color: white;")
        self.symulacja_aktywna = True
        self.awaria_wyswietlona = False
        self.utworz_interfejs()
        self.timer = QTimer(); self.timer.timeout.connect(self.aktualizuj_gre); self.timer.start(30) #timer uruchamiania funkcji

    def utworz_interfejs(self):
        self.btn_main = QPushButton("STOP", self)
        self.btn_main.setGeometry(20, 20, 100, 40)
        self.btn_main.setStyleSheet("background-color: #d32f2f; font-weight: bold;")
        self.btn_main.clicked.connect(self.przelacz_symulacje)

        self.z1 = Zbiornik(50, 100, 150, 250, "Z1 (Źródło)"); self.z1.ilosc = 80
        self.z2 = Zbiornik(400, 50, 80, 100, "Z2")
        self.z3 = Zbiornik(400, 220, 80, 100, "Z3")
        self.z4 = Zbiornik(550, 390, 80, 100, "Z4 (Parownik)") 
        
        self.zbiorniki = [self.z1, self.z2, self.z3, self.z4]

        self.rura1 = Rura([(self.z1.rect.right(), self.z1.rect.bottom()-20), (300, self.z1.rect.bottom()-20), (300, self.z2.rect.top()), (self.z2.rect.center().x(), self.z2.rect.top())])
        self.rura2 = Rura([(self.z2.rect.right(), self.z2.rect.bottom()-10), (520, self.z2.rect.bottom()-10), (520, self.z3.rect.top()-20), (self.z3.rect.center().x(), self.z3.rect.top())])
        self.rura3 = Rura([(self.z3.rect.right(), self.z3.rect.bottom()-15), (590, self.z3.rect.bottom()-15), (590, self.z4.rect.top()-20), (self.z4.rect.center().x(), self.z4.rect.top())])
        
        self.rury = [self.rura1, self.rura2, self.rura3]

        y = int(self.z1.rect.bottom() + 10); x = int(self.z1.rect.x())
        b1 = QPushButton("+", self); b1.setGeometry(x, y, 70, 30); b1.setStyleSheet("background-color: #555;"); b1.clicked.connect(lambda: self.z1.dodaj_ciecz(20))
        b2 = QPushButton("-", self); b2.setGeometry(x+80, y, 70, 30); b2.setStyleSheet("background-color: #555;"); b2.clicked.connect(lambda: self.z1.usun_ciecz(20))

        self.utworz_zawor(self.z2, 0, "ZAWÓR")
        self.utworz_zawor(self.z3, 0, "ZAWÓR") 

        self.utworz_panel_z4()

    def utworz_zawor(self, zbiornik, offset_y, tekst):
        btn = QPushButton(tekst, self)
        btn.setGeometry(int(zbiornik.rect.x()), int(zbiornik.rect.bottom()+10 + offset_y*35), 100, 30)
        btn.setCheckable(True); btn.setChecked(True); btn.setStyleSheet("background-color: #2e7d32;")
        def obs():
            zbiornik.zawor_otwarty = not zbiornik.zawor_otwarty
            btn.setStyleSheet(f"background-color: {'#2e7d32' if zbiornik.zawor_otwarty else '#c62828'};")
        btn.clicked.connect(obs)

    def utworz_panel_z4(self):
        x, y = int(self.z4.rect.x()), int(self.z4.rect.bottom() + 10)
        self.btn_heat = QPushButton("GRZANIE", self)
        self.btn_heat.setGeometry(x, y, 90, 30)
        self.btn_heat.setCheckable(True)
        self.btn_heat.setStyleSheet("background-color: #442222; border: 1px solid red;")
        self.btn_heat.clicked.connect(self.obsluga_grzania)

    def obsluga_grzania(self):
        self.z4.grzalka_on = self.btn_heat.isChecked()
        self.btn_heat.setStyleSheet(f"background-color: {'red' if self.z4.grzalka_on else '#442222'}; border: 1px solid red;")

    def przelacz_symulacje(self):
        self.symulacja_aktywna = not self.symulacja_aktywna
        self.btn_main.setText("STOP" if self.symulacja_aktywna else "START")

    def aktualizuj_gre(self):
        if self.z4.zniszczony and not self.awaria_wyswietlona: #sprawdzanie awarii
            self.symulacja_aktywna = False; self.btn_main.setText("START"); self.awaria_wyswietlona = True
            if OknoAwarii(self).exec_(): 
                self.z4.napraw(); self.awaria_wyswietlona = False
                if self.btn_heat.isChecked(): self.btn_heat.click()
            return

        if not self.symulacja_aktywna: return

        for z in self.zbiorniki: z.oblicz_fizyke() #przeliczanie fizyki dla zbiorników

        self.przelewaj_wode(self.z1, self.z2, self.rura1, self.z1.zawor_otwarty) #przelewanie wody
        self.przelewaj_wode(self.z2, self.z3, self.rura2, self.z2.zawor_otwarty)
        self.przelewaj_wode(self.z3, self.z4, self.rura3, self.z3.zawor_otwarty)      

        self.update() #odswiezanie

    def przelewaj_wode(self, zrodlo, cel, rura, czy_zawor_otwarty):
        mozna = True
        if zrodlo.ilosc <= 0: mozna = False #nie ma co lac
        if cel.ilosc >= cel.pojemnosc: mozna = False #nie ma miejsca
        if not czy_zawor_otwarty: mozna = False #zamkniety zawor
        
        if mozna: #plyiecie wody
            ilosc = zrodlo.usun_ciecz(PREDKOSC_PRZEPLYWU) 
            wlane = cel.dodaj_ciecz(ilosc, zrodlo.temperatura) 
            if wlane < ilosc: zrodlo.ilosc += (ilosc - wlane) 
            if wlane == 0: rura.ustaw_przeplyw(False); return
            rura.ustaw_przeplyw(True)
        else:
            rura.ustaw_przeplyw(False)

    def paintEvent(self, e):
        p = QPainter(self); p.setRenderHint(QPainter.Antialiasing)
        for r in self.rury: r.draw(p)
        for z in self.zbiorniki: z.draw(p)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SystemSCADA()
    window.show()
    sys.exit(app.exec_())
