import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QDialog, QVBoxLayout
from PyQt5.QtCore import Qt, QTimer, QPointF, QRectF
from PyQt5.QtGui import QPainter, QColor, QPen, QPainterPath, QFont
# stale badz poczatkowe wartosci
TEMP_WRZENIA = 100.0
TEMP_ZAMARZANIA = 0.0
TEMP_OTOCZENIA = 20.0
CISNIENIE_MAX = 100.0

SZYBKOSC_GRZANIA = 1.2
SZYBKOSC_PAROWANIA = 0.5
PREDKOSC_PRZEPLYWU = 2.0
