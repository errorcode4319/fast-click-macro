import sys 
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import dll 
import datetime

class App(QWidget):


    def __init__(self):
        super().__init__()
        self.initUI()

        self.timer = QTimer(self)
        self.timer.setInterval(500)
        self.timer.timeout.connect(self.show_cur_info)
        self.timer.start()

        self.target_host = "-"
        self.server_time = "--:--:--"
        self.target_rtt_ms = 6
        self.click_time = "--:--:--"

        dll.init() 


    def initUI(self):
        self.setWindowTitle('Weverse Submit Macro')
        self.resize(400, 300)
        self.center()

        self.lbl_target_host = self.new_label(10, 10, 70, 25, "서버:")
        self.lbl_click_time = self.new_label(10, 40, 70, 25, "시간:")
        self.le_target_host = self.new_lineedit(80, 10, 240, 20)
        self.le_click_time = self.new_timeedit(80, 40, 240, 20)
        self.btn_target_host = self.new_button(330, 10, 60, 50, "설정")
        self.btn_target_host.clicked.connect(self.set_config)

        self.lbl_target_host_info = self.new_label(10, 70, 380, 25, "")
        self.lbl_server_time_info = self.new_label(10, 100, 380, 25, "")
        self.lbl_server_rtt_info = self.new_label(10, 130, 380, 25, "")
        self.lbl_click_time_info = self.new_label(10, 160, 380, 25, "")

        self.show()


    def set_config(self):
        target_host = self.le_target_host.text()
        click_time = self.le_click_time.text()
        print(f"Target Host: {target_host}, Click Time: {click_time}")
        
        self.target_host = target_host
        self.click_time = click_time
        vals = [int(val) for val in click_time.split(':')]
        # TODO: Set Configure & Macro Restart
        dll.start(target_host, vals[0], vals[1], vals[2])


    def show_cur_info(self):
        # TODO: Get Current Info From Macro Core
        rtt, server_time = dll.get_states()
        self.target_rtt_ms = rtt
        dt = datetime.datetime.fromtimestamp(server_time // 1000)
        self.server_time = str(dt)

        self.lbl_target_host_info.setText(f"타겟 서버: {self.target_host}")
        self.lbl_server_time_info.setText(f"서버 시간(실시간): {self.server_time}")
        self.lbl_server_rtt_info.setText(f"패킷 왕복 시간 측정(RTT): {self.target_rtt_ms}ms")
        self.lbl_click_time_info.setText(f"클릭 설정 시간: {self.click_time}")


    
    def center(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())    


    def new_label(self, x, y, w, h, text: str = ""):
        lbl = QLabel(self)
        lbl.setGeometry(x, y, w, h)
        lbl.setText(text)
        return lbl 

    
    def new_lineedit(self, x, y, w, h):
        le = QLineEdit(self)
        le.setGeometry(x, y, w, h)
        return le 

    
    def new_button(self, x, y, w, h, text: str = ""):
        btn = QPushButton(self)
        btn.setText(text)
        btn.setGeometry(x, y, w, h)
        return btn  


    def new_timeedit(self, x, y, w, h):
        te = QTimeEdit(self)
        te.setTime(QTime.currentTime())
        te.setDisplayFormat('hh:mm:ss')
        te.setGeometry(x, y, w, h)
        return te 


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())