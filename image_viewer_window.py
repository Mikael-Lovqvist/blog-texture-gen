from PySide2.QtWidgets import QApplication, QMainWindow, QSizePolicy
from PySide2.QtGui import QImage, QPainter
from PySide2.QtCore import Qt, QSize
#bare bones - we should add multiple formats, scrolling and so forth

class viewer_window(QMainWindow):
	def __init__(self, title=None):
		self.image = None
		self.zoom = 1.0
		self.zoom_exp = 0
		self.antialiasing = False
		self.keyboard_event_handlers = (self.__class__.default_keyboard_handler,)	#note, we treat all handlers as unbound to make it easier to add other functions - this may change in the future though

		super().__init__(flags=Qt.Dialog)

		if title:
			self.setWindowTitle(title)

		self.setAutoFillBackground(True)
		self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

	def default_keyboard_handler(self, event):
		if event.key() == Qt.Key_Plus:
			self.zoom_exp += .25
			self.zoom = 2.0 ** self.zoom_exp
			print(self.zoom)
			self.update()
		elif event.key() == Qt.Key_Minus:
			self.zoom_exp -= .25
			self.zoom = 2.0 ** self.zoom_exp
			print(self.zoom)
			self.update()
		elif event.key() == Qt.Key_A:
			self.antialiasing = not self.antialiasing
			self.update()


	def keyPressEvent(self, event):
		for kbd_handler in self.keyboard_event_handlers:
			status = kbd_handler(self, event)

			#Todo - we should return early for certain status but we have not named/defined these yet so we will go through all handlers

	def new_pil_image(self, image):

		width, height = image.size

		bytes_per_line = width * 3
		format = QImage.Format_RGB888
		self.image = QImage(image.tobytes(), width, height, bytes_per_line, format)
		self.update()

	def new_raw_image(self, width, height, bytes_per_line, data, format):
		self.image = QImage(data, width, height, bytes_per_line, format)
		self.update()

	def paintEvent(self, event):
		if self.image:
			painter = QPainter(self)
			#center
			geometry = self.geometry()
			iw, ih = self.image.width(), self.image.height()
			ww, wh = geometry.width(), geometry.height()

			#rzoom = 1.0 / self.zoom

			x, y = (ww - iw * self.zoom) * .5, (wh - ih * self.zoom) * .5
			painter.translate(x, y)
			painter.scale(self.zoom, self.zoom)
			painter.setRenderHint(painter.SmoothPixmapTransform, self.antialiasing)

			painter.drawImage(0, 0, self.image)

	def minimumSizeHint(self):
		return self.image.size()

