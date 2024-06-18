## Model
![Model](https://github.com/illjlack/MyPlayer/assets/104678570/e74a6b1e-90af-4c20-9726-5a23fc21b5ec)

**CacheDownloader（缓存器）**:

- 负责下载和缓存音乐文件、歌词以及图片文件。该组件确保在播放音乐时，有足够的数据缓冲以保证播放流畅。

**MusicInfoManager（管理数据库）**:

- 负责管理本地数据库，存储音乐的相关信息，如音乐名、专辑名、时长、路径、文件类型、歌词、图片和是否喜爱等。
- 提供对数据库的增删改查操作，是音乐信息管理的核心。

**MusicLoader（音乐加载）**:

- 负责从本地文件或网络加载音乐文件及其元数据。
- 利用`CacheDownloader`下载必要的文件，并调用`MusicInfoManager`更新数据库。
- 解析从网络或本地JSON文件中获取的音乐数据，并生成对应的音乐信息对象。

**MusicListManager（管理本地、远端、喜爱列表）**:

- 负责管理音乐列表，包括本地音乐、远端音乐和用户喜爱的音乐列表。
- 与`MusicLoader`和`MusicInfoManager`交互，确保音乐列表的及时更新和管理。

**MusicPlayerList（当前播放列表）**:

- 管理当前播放的音乐列表，提供播放控制功能。
- 与`MusicPlayer`和`LyricAnalyzer`组件交互，确保音乐播放时能够正确显示歌词并进行实时更新。

## View
![View](https://github.com/illjlack/MyPlayer/assets/104678570/6d4b2341-506d-4b0a-86f6-4c646c3d99a3)

**MainWidget**:

- 应用的主窗口，包含所有其他子组件。
- 负责整体布局和协调各个子组件之间的交互。

**BottomWidget（底部）**:

- 负责播放控制，如播放、暂停、停止等操作。
- 显示当前播放的音乐信息。
- 与`PlaySongInfoWidget`交互，显示正在播放的音乐详细信息。

**MiddleWidget（中间部分）**:

- 包含中间部分的所有组件，负责显示音乐列表和详细信息。
- 分为左边列表部分和右边抽屉部分。
- **MiddleWidgetLeft（左边列表）**:
  - 显示本地音乐、远端音乐和喜爱的音乐列表。
  - 允许用户在本地和远端音乐之间切换。
- **MiddleWidgetRightStack（右边抽屉）**:
  - 显示选中的音乐列表的详细信息。
  - 负责在用户选择不同列表时更新显示内容。

**MusicPlayerWidget（音乐播放器）**:

- 显示歌曲详细信息、歌词和专辑封面。
- 负责显示当前播放列表，并支持点击跳转到歌曲详细页面。

**PlaySongInfoWidget（正在播放音乐信息）**:

- 显示正在播放音乐的信息，如歌曲名、歌手、专辑封面等。
- 与`BottomWidget`交互，提供当前播放状态的详细信息。

## 主要交互
![主要交互](https://github.com/illjlack/MyPlayer/assets/104678570/8d6eda91-c8d5-47ed-a6ab-c7564eb60c6a)

# 遇到的问题和解决

### 1.connect时参数不匹配，可以用lambda

```
connect(reply, &QNetworkReply::finished, this, [this, reply]() { this->onFileDownloaded(reply); });
```

### 2.qt的network不支持https,需要openssl

版本："OpenSSL 1.0.2j 26 Sep 2016"，在文件运行路径下加入dll

### 3.ui太复杂，用.ui难画

用.cpp继承QWidget，分各组件设计

### 4.列表难搞，

用代理委托绘制，用鼠标点击位置获得点击的信息

### 5.中文字符乱码，读取txt乱码

好像是qt用utf-8,vs用gbk的问题，统一用utf-8

每个头文件加

```
#pragma execution_character_set("utf-8")
```

txt读取时设置

```
QFile file(music.lyric);
if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    in.setCodec("UTF-8"); // 设置为UTF-8编码
    QString lyrics = in.readAll();
    file.close();
    parseLyrics(lyrics);
}
```

### 6.数据库创建失败，缺少提示，很久才确定是数据库创建的问题

打包后，测试，发现经常第一次很多功能没用，观察后发现没用数据库文件，中间使用了数据库错误提示，然后尝试了使用内存加载数据库，能正常运行，然后注意到第二用就可以了，最后发现，在AppData下的应用目录在创建数据库时，还没创建。

（以后访问路径要测试）



# 功能介绍

### 1.双击播放或者单击播放列表或者右键菜单选择播放，添加到列表会到播放列表末尾
![image-20240617030337173](https://github.com/illjlack/MyPlayer/assets/104678570/35a4af24-2219-431d-bb63-aa35e4632ca2)

![image-20240617030409198](https://github.com/illjlack/MyPlayer/assets/104678570/c55444f8-526f-4c4a-bf75-11adb271435a)

### 2.本地导入，直接导入mp3或者wav,自动解析时间，和json导入，json导入的格式和git上的一致
![image-20240617030647117](https://github.com/illjlack/MyPlayer/assets/104678570/48828d8a-d82f-4c2f-b5d2-e92476febeae)
![image-20240617030724590](https://github.com/illjlack/MyPlayer/assets/104678570/e7cb2699-ea0f-4b64-b582-c95eb5a8e4e8)
![image-20240617030743678](https://github.com/illjlack/MyPlayer/assets/104678570/88aff3e8-aa10-471d-b092-19547fed9e66)


### 3.歌曲详细页，歌词实时播放，背景为模糊的专辑图（点击专辑图片或者词）
![image-20240617030829600](https://github.com/illjlack/MyPlayer/assets/104678570/eb230686-badc-4f9f-8597-8e74cb4ae4f8)
![image-20240617030932056](https://github.com/illjlack/MyPlayer/assets/104678570/33b8d28b-5e02-4ac9-bf6d-ce7e6ac22383)

### 4.最小化托盘，通过进程监听保证计算机上一个实例，再打开.exe会唤醒最小化的实例
![image-20240617030907107](https://github.com/illjlack/MyPlayer/assets/104678570/17b560f6-fbd7-4976-87d8-a4d4bcb935d3)

### 5.查看播放列表，可以选择顺序，随机，循环，单曲循环
![image-20240617031005258](https://github.com/illjlack/MyPlayer/assets/104678570/447a5d46-3b92-4e14-a664-2d385d4680f3)

### 6.喜爱，喜爱有列表，记录保存在数据库
![image-20240617031052037](https://github.com/illjlack/MyPlayer/assets/104678570/0e834208-6e6a-42cd-aa70-5f6ac2ecac32)




# 运行

把openssl的动态库放到.exe的运行目录下









# 改进地方

第一次，写的有点烂，可以参考重构一下

有先偷偷下载。

可以缓存歌曲部分以加速

按钮的逻辑有些不对，可以接受到播放状态的信号再改变，而不是点击就变

可以爬网易等的网络音乐参考：

```
//main.py
import requests

# 目前无法通过接口来直接来获得所有的记录，只能每一次进行10条的访问

song_name = 'majiko'

# 请求URL
url = 'https://www.jbsou.cn/'

# 请求头部，根据您提供的头部信息配置
headers = {
    'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8',
    'Accept': 'application/json, text/javascript, */*; q=0.01',
    'Origin': 'https://www.jbsou.cn',
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36 Edg/125.0.0.0',
    'X-Requested-With': 'XMLHttpRequest',
}

# POST请求的数据
data = {
    'input': song_name,
    'filter': 'name',
    'type': 'netease',
    'page': 1
}

# 发送POST请求
response = requests.post(url, headers=headers, data=data)
# print(response.text.encode('utf-8'))

# 打印响应的JSON数据
json_data = response.json()
# print(json_data)

print(response.json())
# for item in json_data['data']:
#     title = item['title']
#     author = item['author']
#     url = item['url']
#     pic = item['pic']
#     print(f"标题: {title}, 作者: {author}, 音频链接: {url}, 封面图片: {pic}")





//get_lyric.py
import requests
import json

def get_lyric(song_id):
    headers = {
        "user-agent" : "Mozilla/5.0",
        "Referer" : "http://music.163.com",
        "Host" : "music.163.com"
    }
    if not isinstance(song_id, str):
        song_id = str(song_id)
    url = f"http://music.163.com/api/song/lyric?id={song_id}+&lv=1&tv=-1"
    r = requests.get(url, headers=headers)
    r.raise_for_status()
    r.encoding = r.apparent_encoding
    json_obj = json.loads(r.text)
    return json_obj["lrc"]["lyric"]

lyric = get_lyric(28427775)
print(lyric)


```

