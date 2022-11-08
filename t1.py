#This experiment is about optimizing the ray marching so that we can skip sub cubes from the outside in
from efforting.tech.qt.widgets.image_viewer_window import viewer_window, QImage, Qt, QSize
import random
import time, math, os, subprocess
from PySide2.QtWidgets import QApplication
import threading, queue

import ctypes

# w = 2048
# h = 1152

w = 1024
h = 1024


INFINITE = object()


class image_renderer:


	@classmethod
	def get_start_time(cls):
		return time.monotonic()

	@classmethod
	def get_current_time(cls):
		return time.monotonic()

	@classmethod
	def setup_rendering(cls, renderer):
		cls.renderer = renderer

	@classmethod
	def new_raw_image(cls, width, height, line_stride, buffer, image_format):
		cls.monitor.new_raw_image(width, height, line_stride, buffer, image_format)

	@classmethod
	def get_render_target(cls, monitor):
		cls.monitor = monitor
		return cls

rendering_mode = image_renderer

optimizations = (
	'-fno-math-errno',
	'-fassociative-math',
	'-freciprocal-math',
	'-ffinite-math-only'
)

class render_thread(threading.Thread):
	def __init__(self, renderer):
		super().__init__()
		self.renderer = renderer
		self.finished = threading.Event()

	def run(self):
		while True:
			pending_op = self.renderer.get_next_render_operation()
			if pending_op is None:
				return

			self.renderer.render_line(pending_op)
			self.renderer.line_complete()


class renderer:
	def __init__(self, width, height, thread_count=None):
		self.width, self.height = width, height

		self.state_lock = threading.Lock()
		if thread_count is not None:
			self.thread_count = thread_count
		else:
			self.thread_count = len(os.sched_getaffinity(0))

		self.render_operations = queue.Queue(self.thread_count * 2)
		self.frame_finished = threading.Event()
		self.started = False

	def start(self):
		with self.state_lock:
			self.thread_pool = tuple(render_thread(self) for _ in range(self.thread_count))
			for thread in self.thread_pool:
				thread.start()

			self.started = True

	def stop(self):
		for _ in self.thread_pool:
			self.render_operations.put(None)	#termination sentinel


	def join(self):
		for thread in self.thread_pool:
			thread.join()


	def get_next_render_operation(self):
		return self.render_operations.get()

	def render_frame(self, *positionals, **named):
		assert self.started
		self.frame_finished.clear()
		with self.state_lock:
			self.lines_completed = 0

		self.begin_frame(*positionals, **named)

		for line in iter(range(self.height)):
			self.render_operations.put(line)

		self.frame_finished.wait()
		self.end_frame()


	def line_complete(self):
		with self.state_lock:
			self.lines_completed += 1

		if self.lines_completed == self.height:
			self.frame_finished.set()


class loop_iterator(tuple):
	def __init__(self, items):
		self.position = 0
		self.items = items

	def __next__(self):
		pending = self.items[self.position]
		self.position = (self.position + 1) % len(self.items)

		return pending

def field_structure(**named_entries):
	return tuple(named_entries.items())

class vec2(ctypes.Structure):
	_fields_ = field_structure(
		x = ctypes.c_float,
		y = ctypes.c_float,
	)


class rendering_point(ctypes.Structure):
	_fields_ = field_structure(
		position = vec2,
		weight = ctypes.c_float,
	)

class rendering_settings(ctypes.Structure):
	_fields_ = field_structure(
		num_points = ctypes.c_int,
		points = ctypes.POINTER(rendering_point),
	)


class test_renderer(renderer):

	def init_renderer(self, render_target):
		frame_type = (rgb_pixel * w * h)
		self.render_target = render_target
		self.render_buffer_pool = loop_iterator((	#Double buffering
			frame_type(),
			frame_type(),
		))
		self.line_stride = ctypes.sizeof(self.render_buffer_pool[0][0])

		assert subprocess.call(('gcc', '-fPIC',  '--shared', 'renderer.c', '-lm', '-o', 'renderer.so', f'-DWIDTH={w}', f'-DHEIGHT={h}', '-march=native', '-Ofast', *optimizations)) == 0

		self.lib = ctypes.CDLL('./renderer.so')

		self.lib.render_line.argtypes = (
			frame_type,
			ctypes.c_int,
			ctypes.POINTER(rendering_settings),
		)



		#Create some random points
		num_points = 50
		point_data = (rendering_point * num_points)()	#Create type and initialize

		#Fill in data for the points
		for p in point_data:
			p.position.x = random.uniform(0, 1)
			p.position.y = random.uniform(0, 1)
			p.weight = random.uniform(-1, 1)


		#Setup rendering_settings
		self.rendering_settings = rendering_settings(
			num_points,
			point_data,
		)






	def begin_frame(self, frame_time):
		self.current_buffer = next(self.render_buffer_pool)
		self.frame_time = frame_time


	def end_frame(self):
		self.render_target.new_raw_image(self.width, self.height, self.line_stride, self.current_buffer, QImage.Format_RGB888)

	def render_line(self, y):
		self.lib.render_line(self.current_buffer, y, self.rendering_settings)


class rgb_pixel(ctypes.Structure):
	_fields_ = (
		('r', ctypes.c_ubyte),
		('g', ctypes.c_ubyte),
		('b', ctypes.c_ubyte),
	)

#w = 192 * 10
#h = 108 * 10


app = QApplication()
mw = viewer_window('Image Viewer')
mw.setStyleSheet('''
	QMainWindow {
		background-color: #222;
	}
''')

r = test_renderer(w, h)
r.init_renderer(rendering_mode.get_render_target(mw))

#mw.sizeHint = lambda: QSize(w, h)
mw.resize(w, h)

r.start()
mw.show()



should_terminate = False

def abort_things(*_):
	global should_terminate
	should_terminate = True

def keyboard_event_handler(window, event):
	if event.key() == Qt.Key_Escape:
		abort_things()



	# elif event.key() == Qt.Key_N:
	# 	r.lib.next_sponge()
	# elif event.key() == Qt.Key_Period:
	# 	r.lib.increase_rotspeed()
	# elif event.key() == Qt.Key_Comma:
	# 	r.lib.decrease_rotspeed()
	# elif event.key() == Qt.Key_R:
	# 	r.lib.toggle_rotating()

	# elif event.key() == Qt.Key_Colon:
	# 	r.lib.increase_lightspeed()
	# elif event.key() == Qt.Key_Semicolon:
	# 	r.lib.decrease_lightspeed()
	# elif event.key() == Qt.Key_L:
	# 	r.lib.toggle_movelight()


	# elif event.key() == Qt.Key_Greater:
	# 	r.lib.more_levels()
	# elif event.key() == Qt.Key_Less:
	# 	r.lib.less_levels()

	# elif event.key() == Qt.Key_A:
	# 	r.lib.camera_closer()
	# elif event.key() == Qt.Key_Z:
	# 	r.lib.camera_further()

mw.keyboard_event_handlers += (keyboard_event_handler,)
mw.closeEvent = abort_things
rendering_mode.abort_callable = abort_things



rendering_mode.setup_rendering(r)

class continuous_render(threading.Thread):
	def run(self):
		last_fps_update = rendering_mode.get_start_time()
		frame_counter = 0
		while not should_terminate:
			frame_counter += 1
			now = rendering_mode.get_current_time()
			r.render_frame(now)

			if now - last_fps_update >= 1.0:
				last_fps_update = now
				print('FPS', frame_counter)
				frame_counter = 0

		# Abort renderer
		r.stop()
		mw.close()



import signal
signal.signal(signal.SIGINT, abort_things)

cr = continuous_render()
cr.start()

app.exec_()