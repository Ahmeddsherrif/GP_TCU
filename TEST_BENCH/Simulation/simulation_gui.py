import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QVBoxLayout, QHBoxLayout, QPushButton
from PyQt5.QtCore import Qt, QTimer


class Application(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Serial Control")
    
        # Create labels and numbers
        self.labels = ['A', 'B', 'C', 'D']
        self.number_labels = []
        self.number_vars = []

        layout = QVBoxLayout()

        # Create and add the "Hello" label
        hello_label = QLabel("IDLE")
        hello_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(hello_label)
        
        for label in self.labels:
            label_text = f"{label}:"
            number_var = 0
            self.number_vars.append(number_var)

            label_widget = QLabel(label_text)
            number_label = QLabel(str(number_var))
            self.number_labels.append(number_label)

            hbox = QHBoxLayout()
            hbox.addWidget(label_widget)
            hbox.addWidget(number_label)
            layout.addLayout(hbox)

        # Create buttons
        button_layout = QHBoxLayout()
        
        button_start = QPushButton("Start")
        button_stop = QPushButton("Stop")
        button_reset = QPushButton("Reset")
        
        button_layout.addWidget(button_start)
        button_layout.addWidget(button_stop)
        button_layout.addWidget(button_reset)
        
        layout.addLayout(button_layout)
        self.setLayout(layout)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Application()
    window.show()
   
    
    # thread to run the state machine
    app.exec_()
    
    print("hello")