import sensor, image, time, math
from pyb import UART, LED, Pin, Timer

threshold_for_court = (45, 98, -22, 18, -19, 43)# コートの色取り用変数 (53, 96, -63, -19, -38, 23)
threshold_for_goal_yellow =(63, 91, -20, 6, 60, 83)# ゴールの色取り用変数(黄色) (92, 98, -31, 2, 22, 96)
threshold_for_goal_blue = (17, 45, -21, 33, -61, -27) # ゴールの色取り用変数(青色) (17, 45, -21, 33, -61, -27)

screen_center = [129, 136]                  # 画面の中央座標
sensor.reset()
sensor.set_pixformat(sensor.RGB565)#カラースケール
sensor.set_framesize(sensor.QVGA)#解像度Ss
sensor.skip_frames(time = 2000)
#sensor.set_contrast(1)#コントラスト
#sensor.set_brightness(-3)#明るさ
#sensor.set_saturation(2)#彩3~-3
sensor.skip_frames(time = 1000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_exposure(False)
sensor.set_auto_whitebal(False)
uart = UART(3, 115200, timeout_char=250)

clock = time.clock()

timer = Timer(4, freq=1000)

while(True):
    clock.tick()
    try:
        img = sensor.snapshot()
    except RuntimeError as e:
        print(e)

    img.gamma(gamma=1.0,contrast=1.3,brightness= -0.05)#gamma,contrast,brightness



    img.draw_cross(screen_center[0], screen_center[1])    # クロスヘアの表示
    img.draw_circle(screen_center[0],screen_center[1],180,[0,0,0],140)

    #=======================変数定義ライン=======================

    read_count_court            =   0                               # コートの色を何度取ったかを代入する変数
    read_count_goal_yellow      =   0                               # 黄ゴールの色を何度取ったかを代入する変数
    read_count_goal_blue        =   0                               # 青ゴールの色を何度取ったかを代入する変数

    maximum_cx_court            =   0                               # コートの最大色取りサイズの中心x座標
    maximum_cy_court            =   0                               # コートの最大色取りサイズの中心y座標
    maximum_area_court          =   0                               # コートの最大色取りサイズのエリアサイズ

    maximum_cx_goal_yellow      =   0                               # 黄ゴールの最大色取りサイズの中心x座標
    maximum_cy_goal_yellow      =   0                               # 黄ゴールの最大色取りサイズの中心y座標
    maximum_area_goal_yellow    =   0                               # 黄ゴールの最大色取りサイズのエリアサイズ

    maximum_cx_goal_blue        =   0                               # 青ゴールの最大色取りサイズの中心x座標
    maximum_cy_goal_blue        =   0                               # 青ゴールの最大色取りサイズの中心y座標
    maximum_area_goal_blue      =   0                               # 青ゴールの最大色取りサイズのエリアサイズ

    court_deg                   =   0                               # 画面中央からのコートの角度
    court_distance              =   0                               # 画面中央からのコートの距離
    goal_yellow_deg             =   0                               # 画面中央からの黄ゴールの角度
    goal_yellow_distance        =   0                               # 画面中央からの黄ゴールの距離
    goal_blue_deg               =   0                               # 画面中央からの青ゴールの角度
    goal_blue_distance          =   0                               # 画面中央からの青ゴールの距離

    #=======================配列定義ライン=======================

    cx_court                    =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # コートの中心x座標保存用配列
    cy_court                    =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # コートの中心y座標保存用配列
    area_court                  =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # コートの色取りエリア保存用配列

    cx_goal_yellow              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの中心x座標保存用配列
    cy_goal_yellow              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの中心y座標保存用配列
    area_goal_yellow            =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 黄ゴールの色取りエリア保存用配列

    cx_goal_blue                =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 青ゴールの中心x座標保存用配列
    cy_goal_blue                =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 青ゴールの中心y座標保存用配列
    area_goal_blue              =   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]      # 青ゴールの色取りエリア保存用配列

    #=======================黄ゴール色取りライン=======================

    for blob in img.find_blobs([threshold_for_goal_yellow], pixels_threshold=10, area_threshold=15, merge=True,margin=25):
        if read_count_goal_yellow + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_goal_yellow += 1

        cx_goal_yellow[read_count_goal_yellow] = blob.cx()
        cy_goal_yellow[read_count_goal_yellow] = blob.cy()
        area_goal_yellow[read_count_goal_yellow] = blob.area()

    for blob in img.find_blobs([threshold_for_goal_blue], pixels_threshold=10, area_threshold=15, merge=True,margin=25):
        if read_count_goal_blue + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_goal_blue += 1

        cx_goal_blue[read_count_goal_blue] = blob.cx()
        cy_goal_blue[read_count_goal_blue] = blob.cy()
        area_goal_blue[read_count_goal_blue] = blob.area()

    for blob in img.find_blobs([threshold_for_court], pixels_threshold=10, area_threshold=20, merge=True,margin=40):
        if read_count_court + 1 >= 10:              # コートの色を10回以上取った場合、それ以上コートの色取りをしない。
            break
        else:                                   # まだコートの色取りが10回行われていない場合、読み取り回数を増やす。
            read_count_court += 1

        cx_court[read_count_court] = blob.cx()
        cy_court[read_count_court] = blob.cy()
        area_court[read_count_court] = blob.area()
        img.draw_rectangle(blob.rect());

    #==============================================

    maximum_cx_goal_yellow = (max(cx_goal_yellow[:]))
    maximum_cy_goal_yellow = (max(cy_goal_yellow[:]))
    maximum_area_goal_yellow = (max(area_goal_yellow[:]))

    for i in range(0, 9):
        if area_goal_yellow[i] == maximum_area_goal_yellow:
            maximum_cx_goal_yellow = cx_goal_yellow[i]
            maximum_cy_goal_yellow = cy_goal_yellow[i]
            break


    img.draw_cross(maximum_cx_goal_yellow, maximum_cy_goal_yellow)    # コートの中心を交差線で描画
    img.draw_line(screen_center[0], screen_center[1], maximum_cx_goal_yellow, maximum_cy_goal_yellow)  # 画面中心からコート中心へのライン描画

    maximum_cx_goal_blue = (max(cx_goal_blue[:]))
    maximum_cy_goal_blue = (max(cy_goal_blue[:]))
    maximum_area_goal_blue = (max(area_goal_blue[:]))

    for i in range(0, 9):
        if area_goal_blue[i] == maximum_area_goal_blue:
            maximum_cx_goal_blue = cx_goal_blue[i]
            maximum_cy_goal_blue = cy_goal_blue[i]
            break

    img.draw_cross(maximum_cx_goal_blue, maximum_cy_goal_blue)    # コートの中心を交差線で描画
    img.draw_line(screen_center[0], screen_center[1], maximum_cx_goal_blue, maximum_cy_goal_blue)  # 画面中心からコート中心へのライン描画

    maximum_cx_court = (max(cx_court[:]))
    maximum_cy_court = (max(cy_court[:]))
    maximum_area_court = (max(area_court[:]))

    for i in range(0, 9):
        if area_court[i] == maximum_area_court:
            maximum_cx_court = cx_court[i]
            maximum_cy_court = cy_court[i]
            break

    img.draw_cross(maximum_cx_court, maximum_cy_court)    # コートの中心を交差線で描画
    img.draw_line(screen_center[0], screen_center[1], maximum_cx_court, maximum_cy_court)  # 画面中心からコート中心へのライン描画

    #======================計算フェーズ=======================

    #==============================================

    goal_yellow_deg = math.atan2((maximum_cx_goal_yellow - screen_center[0]), (maximum_cy_goal_yellow - screen_center[1]))
    if goal_yellow_deg < 0:
        goal_yellow_deg = (2 * math.pi) - abs(goal_yellow_deg)
    goal_yellow_deg = (math.floor(goal_yellow_deg / (2 * math.pi) * 180))

    goal_yellow_distance = math.sqrt(math.pow((maximum_cx_goal_yellow - screen_center[0]), 2) + math.pow((maximum_cy_goal_yellow - screen_center[1]), 2));


    goal_blue_deg = math.atan2((maximum_cx_goal_blue - screen_center[0]), (maximum_cy_goal_blue - screen_center[1]))
    if goal_blue_deg < 0:
        goal_blue_deg = (2 * math.pi) - abs(goal_blue_deg)
    goal_blue_deg = (math.floor(goal_blue_deg / (2 * math.pi) * 180))

    goal_blue_distance = math.sqrt(math.pow((maximum_cx_goal_blue - screen_center[0]), 2) + math.pow((maximum_cy_goal_blue - screen_center[1]), 2));

    court_deg = math.atan2((maximum_cx_court - screen_center[0]), (maximum_cy_court - screen_center[1]))
    if court_deg < 0:
        court_deg = (2 * math.pi) - abs(court_deg)
    court_deg = (math.floor(court_deg / (2 * math.pi) * 180))

    court_distance = math.sqrt(math.pow((maximum_cx_court - screen_center[0]), 2) + math.pow((maximum_cy_court - screen_center[1]), 2));

    #==============================================

    #======================出力フェーズ=======================

    if maximum_area_goal_yellow == 0:
        goal_yellow_deg = 255
        goal_yellow_distance = 255

    if maximum_area_goal_blue == 0:
        goal_blue_deg = 255
        goal_blue_distance = 255

    uart.write(str(goal_yellow_deg))
    uart.write("a")
    uart.write(str(goal_yellow_distance))
    uart.write("b")
    uart.write(str(goal_blue_deg))
    uart.write("c")
    uart.write(str(goal_blue_distance))
    uart.write("d")
    uart.write(str(court_deg))
    uart.write("e")
    uart.write(str(court_distance))
    uart.write("f")







