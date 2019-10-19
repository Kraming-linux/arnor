import cv2
import numpy as np  # 友情客串（图片可以用一堆数组表示，用这个可以更好处理图片）
#          D:/pictures/sunny.jpg      # 放图片的路径   # 统一把图片放这
#   脚本路径 ： C:/Users/kraming/.PyCharmCE2018.3/config/scratches/scratch_7.py
#    __file__ 是用来获得模块所在的路径的，这可能得到的是一个相对路径 就是找这个文件在哪运行的


def get_info(image):
    print(type(image))  # 数据的类型
    print(image.shape)  # 图像的长，宽，几个通道 (888, 592, 3) 3通道的
    print(image.size)   # 图像数据（总像素大小） 1577088 = 888*592*3  是这么来的
    print(image.dtype)  # 占的位数  就是一个像素点有三个通道，每个通道占多少位数（1个字节uint8）
    pixel_data = np.array(image)   # 用numpy的方法更快显示图片的数组信息
    print(pixel_data)


def get_video():    # 这个可以打开笔记本摄像头
    capture = cv2.VideoCapture(0)  # 捕获摄像头
    running = True
    while running:
        ret, frame = capture.read()  # 读取摄像头的帧数和图像
        cv2.imshow("video", frame)  # 显示在桌面上
        c = cv2.waitKey(50)  # 50ms
        if c == 27:
            running = False


src = cv2.imread("D:/pictures/sunny.jpg")  # 用双引号填写图片路径 不然他会默认为这个文件目录就是要找的图片（用双引号就对了）
cv2.imshow('src', src)      # show你的图片 注意有两个参数 第一个是你要命名的名字 第二个是图片
get_info(src)
# get_video()  # 这个语法莫要瞎用
# gray = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)  # 图像的灰度处理
# cv2.imwrite("D:/pictures/sunnyGray.png", gray)  # 保存图像的方法，可以选择用什么格式（如jpg，png等格式）

cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口

