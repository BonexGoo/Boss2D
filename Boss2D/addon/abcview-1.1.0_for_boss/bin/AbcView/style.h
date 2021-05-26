const char* STYLE ="\
* {\
    background: #222;\
    color: #aaa;\
    font: verdana, arial, helvetica;\
    font-size: 10pt;\
}\
QGroupBox {\
    border: 0px;\
    margin: 0px;\
    padding: 0px;\
    background: #333;\
}\
QPlainTextEdit {\
    background: #222;\
    color: #aaabac;\
}\
QHeaderView::section {\
    background-image: none;\
    background: #444;\
    color: #777;\
    font-weight: bold;\
    font-size: 11px;\
    font-family: Arial;\
    padding-left: 5px;\
    height: 15px;\
    border: 0px;\
    margin: 0px;\
}\
QTreeView {\
    background-color: #373737;\
    border: 0;\
    outline: 0;\
    show-decoration-selected: 1;\
}\
QTreeView::item {\
    color: #cccdce;\
    border-bottom: 1px solid #333;\
    border-right-color: transparent;\
    border-top-color: transparent;\
    outline: 0;\
    height: 20px;\
}\
QTreeView::item:selected {\
    background: #56575b;\
    color: #afa;\
    border: 0;\
}\
QTreeView::item:selected:active {\
    background: #468a4b;\
    border: 0;\
    outline: 0;\
}\
QTreeView::indicator {\
    padding-left: -9px;\
}\
/*QTreeView::indicator:checked {\
    image: url(%(icons)s/eye.png);\
}\
QTreeView::indicator:unchecked {\
    image: url(%(icons)s/eye-off.png);\
}\
QTreeView::branch {\
    width: 0px;\
    background-color: #373737;\
}\
QTreeView::branch:selected {\
    background-color: #56575b;\
}\
QTreeView::branch:has-siblings:!adjoins-item {\
    border-image: url(%(icons)s/vline.png) 0;\
}\
QTreeView::branch:has-siblings:adjoins-item {\
    border-image: url(%(icons)s/branch-more.png) 0;\
}\
QTreeView::branch:!has-children:!has-siblings:adjoins-item {\
    border-image: url(%(icons)s/branch-end.png) 0;\
}\
QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings {\
     border-image: none;\
     image: url(%(icons)s/branch-closed.png);\
}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings  {\
     border-image: none;\
     image: url(%(icons)s/branch-open.png);\
}*/\
QMenuBar {\
    background: #333;\
    color: #aaa;\
}\
QMenuBar::item {\
    background: #333;\
    color: #aaa;\
}\
QMenuBar::item:selected {\
    background: #111;\
    color: #aaa;\
}\
QMenu {\
    background-color: #111;\
    border: 1px solid #454545;\
    border-top: 0px;\
    margin: 1px;\
}\
QMenu::item {\
    padding: 2px 25px 2px 20px;\
    color: #aaa;\
    border: 0px;\
}\
QMenu::item:non-exclusive {\
    text-decoration: none;\
}\
QMenu::item:exclusive {\
    font: italic;\
}\
QMenu::item:selected {\
    background: #323332;\
    color: #acadac;\
}\
QMenu::icon:checked {\
    background: gray;\
    border: 1px inset gray;\
    position: absolute;\
    top: 1px;\
    right: 1px;\
    bottom: 1px;\
    left: 1px;\
}\
QMenu::separator {\
    height: 1px;\
    background: #555;\
    margin-left: 10px;\
    margin-right: 5px;\
}\
QMenu::indicator {\
    width: 13px;\
    height: 13px;\
}\
/*QMenu::indicator:non-exclusive:unchecked {\
    image: url(%(icons)s/unchecked.png);\
}\
QMenu::indicator:non-exclusive:checked {\
    image: url(%(icons)s/checked.png);\
}\
QMenu::indicator:exclusive:unchecked {\
    image: url(%(icons)s/unchecked.png);\
}\
QMenu::indicator:exclusive:checked {\
    image: url(%(icons)s/checked.png);\
}*/\
QLineEdit {\
    border: 0px;\
    background: #666;\
    color: #3e6;\
}\
QToolBar {\
    background: #111;\
    spacing: 0px;\
    padding: 0px;\
}\
QToolButton {\
    color: #444;\
}\
QSplitter {\
    background: #444;\
}\
QSplitter::handle {\
    image: none;\
}\
QSplitter::handle:hover {\
    background: #7a7;\
}\
QSplitter::handle:horizontal {\
    width: 3px;\
}\
QSplitter::handle:vertical {\
    height: 3px;\
}\
QProgressBar {\
    border: 0px;\
    background: #444;\
    height: 8px;\
}\
QProgressBar::chunk {\
    background: #4a4;\
}\
QScrollBar:vertical {\
    border: 0ox;\
    background: #373737;\
    width: 10px;\
}\
QScrollBar:horizontal {\
    border: 0px;\
    background: #373737;\
    height: 10px;\
}\
QScrollBar::handle {\
    background: #222;\
    min-width: 20px;\
    border-radius: 5px;\
}\
QScrollBar::add-line {\
    background: #373737;\
}\
QScrollBar::sub-line {\
    background: #373737;\
}\
QSlider::groove:horizontal {\
    background: #4a9;\
    border: 9px solid #333;\
    border-radius: 4px;\
    margin-left: 0px;\
    margin-right: 0px;\
}\
QSlider::handle:horizontal {\
    background: #4a9;\
    width: 30px;\
}\
#time_slider QPushButton {\
    background: #222;\
    color: #aaa;\
    border: 0;\
    outline: 0;\
}\
#time_slider QPushButton:hover {\
    background: #353637;\
}\
#time_slider QLabel {\
    margin-top: 2px;\
    color: #888;\
}\
#time_slider QLineEdit {\
    border: 0px;\
    background: #373737;\
    color: #3e6;\
}\
#time_slider QLineEdit:hover {\
    background: #665537;\
}\
/*#time_slider #play_button {\
    background: url(%(icons)s/play.png) center no-repeat;\
    width: 50px;\
    height: 15px;\
}\
#time_slider #stop_button {\
    background: url(%(icons)s/stop.png) center no-repeat;\
    width: 50px;\
    height: 15px;\
}*/\
QMenu QPushButton {\
    background: #222;\
    color: #acadac;\
    text-align: left;\
    border: 0px;\
    margin: 0px;\
    padding: 4 6 4 20;\
}\
#edit_button {\
    background: #222;\
    padding: 0px;\
}\
QMenu QPushButton:hover {\
    background: #323332;\
}\
QDockWidget {\
    border: 1px solid lightgray;\
    /*titlebar-close-icon: url(close.png);\
    titlebar-normal-icon: url(undock.png);*/\
}\
QDockWidget::title {\
    text-align: left;\
    color: #AAA;\
    padding-left: 5px;\
}\
";
