// BarChart.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "BarChart.h"

using namespace std;

#define MAX_LOADSTRING 100

class ChartData;

class UnitData {
    friend ChartData;
public:
    /// <summary>
    /// 设置Unit的坐标
    /// </summary>
    /// <param name="x">: 一个整数, 代表坐标</param>
    void SetXPos(int x) { this->X = x; return; }

    /// <summary>
    /// 设置Unit下方的文本
    /// </summary>
    /// <param name="str">：一个字符串，长度有限</param>
    void SetText(string str) { this->Text = str; }

    bool SetBarText(int pos, string Str) {
        if (pos >= 0 && pos < EachBarText.size()) {
            EachBarText.emplace(EachBarText.begin() + pos, Str);
            return true;
        }
        return false;
    }

    bool SetBarColor(int pos, COLORREF Color) {
        if (pos >= 0 && pos < EachBarColor.size()) {
            EachBarColor.emplace(EachBarColor.begin() + pos, Color);
            return true;
        }
        return false;
    }

    /// <summary>
    /// 插入一个Bar到指定位置（默认为尾部）
    /// </summary>
    /// <param name="value">：Bar的数值，或理解为Y轴的坐标</param>
    /// <param name="str">：Bar的显示文本</param>
    /// <param name="Color">：Bar的颜色</param>
    /// <param name="pos">：当填写时，保证[0 &lt; pos &lt; size]</param>
    /// <returns>bool类型的值: [true]成功, [false]失败</returns>
    bool InsertBar(int value, string str, COLORREF Color, int pos = -1) {
        if (pos == -1) {
            EachBarData.emplace_back(value);
            EachBarText.emplace_back(str);
            EachBarColor.emplace_back(Color);
            return true;
        }
        if (pos >= 0 && pos < EachBarData.size()) {
            EachBarData.emplace(EachBarData.begin() + pos, value);
            EachBarText.emplace(EachBarText.begin() + pos, str);
            EachBarColor.emplace(EachBarColor.begin() + pos, Color);
            return true;
        }
        return false;
    }

    /// <summary>
    /// 清空
    /// </summary>
    void clear() {
        EachBarData.clear();
        EachBarText.clear();
        int X = -1;
        string Text = "";
    }

    /// <summary>
    /// 获取Unit的X坐标
    /// </summary>
    /// <returns>一个整数</returns>
    int GetXPos() const { return this->X; }

    /// <summary>
    /// 获取Unit下方的文本
    /// </summary>
    /// <returns>一个字符串</returns>
    string GetText() const { return this->Text; }

    /// <summary>
    /// 获取所有Bar的数据
    /// </summary>
    /// <returns>一个vector<int>，包含所有Bar的数据</returns>
    vector<int> GetBarData() const { return this->EachBarData; }

    /// <summary>
    /// 获取所有Bar的颜色数据
    /// </summary>
    /// <returns></returns>
    vector<COLORREF> GetBarColors() const { return this->EachBarColor; }

    /// <summary>
    /// 获取指定Bar的颜色数据
    /// </summary>
    /// <param name="i"></param>
    /// <returns></returns>
    COLORREF GetBarColor(int i) const { 
        if (i >= 0 && i < EachBarColor.size()) return EachBarColor[i];
        else return RGB(0, 0, 0);
    }

private:
    vector<int> EachBarData;
    vector<string> EachBarText;
    vector<COLORREF> EachBarColor;
    int X = -1;
    string Text = "";
};

class ChartData {
public:
    /// <summary>
    /// 插入Unit，务必在初始化之后使用
    /// </summary>
    /// <param name="Data">：保证UnitData已初始化</param>
    /// <returns>bool类型: [true]成功, [false]失败</returns>
    bool InsertUnit(UnitData& Data) {
        if (Data.X < 0) return false;
        this->Units.emplace_back(move(Data));
        this->UpdataChar();
        return true;
    }

    /// <summary>
    /// 初始化表格，在进行任何操作前，务必进行初始化（尽量不要二次初始化，否则可能发生未定义行为）
    /// </summary>
    /// <param name="Data">：已有的UnitData，可以接入</param>
    /// <param name="XUnit">：X轴的单位</param>
    /// <param name="YUnit">：Y轴的单位</param>
    /// <param name="XName">：X轴的名称</param>
    /// <param name="YName">：Y轴的名称</param>
    /// <param name="BarWid">：每个Bar的宽度</param>
    /// <param name="Color">：不同Bar的颜色，按顺序读取</param>
    /// <param name="_EnableSample">：是否绘制图例，[true]开，[false]关</param>
    /// <param name="SampleRect">：图例的位置（相对于起始点），只使用left和top参数（如果为默认值则自动生成）请保持right与bottom为0</param>
    /// <param name="hFont">：所有文字的字体</param>
    void InitializeChart(vector<UnitData> Data, int XUnit, int YUnit, string XName, string YName, 
                            int BarWid, HFONT hFont = NULL, bool _EnableSample = true, RECT _SampleRect = { -1,-1,-1,-1 }) {
        this->X_Unit = XUnit;
        this->Y_Unit = YUnit;
        this->X_Name = XName;
        this->Y_Name = YName;
        this->BarWidth = BarWid;

        if (hFont != NULL)
            this->hFont_Axis = hFont;

        if (!Data.empty())
            Units.insert(Units.end(), Data.begin(), Data.end());

        this->YNEnableSample = _EnableSample;

        this->SampleRect = _SampleRect;

        return;
    }

    void EnableSample(bool f) {
        this->YNEnableSample = f;
    }

    /// <summary>
    /// 设置X轴的单位
    /// </summary>
    /// <param name="Unit">：整数</param>
    void SetXUnit(int Unit) { this->X_Unit = Unit; }

    /// <summary>
    /// 设置Y轴的单位
    /// </summary>
    /// <param name="Unit">：整数</param>
    void SetYUnit(int Unit) { this->Y_Unit = Unit; }

    /// <summary>
    /// 设置图例的显示区域
    /// </summary>
    /// <param name="rect">：仅使用left和top</param>
    void SetSampleRect(RECT rect) { this->SampleRect = rect; }

    /// <summary>
    /// 获取X轴坐标长度，坐标长度自动生成，由Bar和Unit的数量的坐标决定
    /// </summary>
    /// <returns></returns>
    int GetXAxisLength() const { return this->X_Axis_Length; }

    /// <summary>
    /// 获取Y轴坐标长度，坐标长度自动生成，由Bar的最大值决定
    /// </summary>
    /// <returns></returns>
    int GetYAxisLength() const { return this->Y_Axis_Length; }

    /// <summary>
    /// 获取X轴的单位
    /// </summary>
    /// <returns></returns>
    int GetXUnit() const { return this->X_Unit; }

    /// <summary>
    /// 获取Y轴的单位
    /// </summary>
    /// <returns></returns>
    int GetYUnit() const { return this->Y_Unit; }

    /// <summary>
    /// 获取Unit的个数
    /// </summary>
    /// <returns></returns>
    size_t GetUnitsCount() const { return this->Units.size(); }

    /// <summary>
    /// 获取指定Unit的数据
    /// </summary>
    /// <param name="i">：当填写时，保证[0 &lt; i &lt; size]</param>
    /// <returns></returns>
    UnitData GetUnitData(int i) const { return this->Units[i]; }

    /// <summary>
    /// 获取X轴的名称
    /// </summary>
    /// <returns></returns>
    string GetXName() const { return this->X_Name; }

    /// <summary>
    /// 获取Y轴的名称
    /// </summary>
    /// <returns></returns>
    string GetYName() const { return this->Y_Name; }

    /// <summary>
    /// 获取Bar的宽度
    /// </summary>
    /// <returns></returns>
    int GetBarWidth() const { return this->BarWidth; }

    /// <summary>
    /// 获取设定的字体
    /// </summary>
    /// <returns></returns>
    HFONT GetAxisFont() const { return this->hFont_Axis; }

    /// <summary>
    /// 获取图例的显示区域
    /// </summary>
    /// <returns>忽略right和bottom参数，left和top分别代表距离起始点的长和高</returns>
    RECT GetSampleRect() const { return this->SampleRect; }

    /// <summary>
    /// 检查图例绘制是否启用
    /// </summary>
    /// <returns></returns>
    bool IsSampleEnable() const { return this->YNEnableSample; }

    /// <summary>
    /// 获取所有的图例
    /// </summary>
    /// <returns></returns>
    vector<pair<COLORREF, string>> GetSamples() const { return this->Samples; }

private:
    /// <summary>
    /// 用于更新坐标轴的长度
    /// </summary>
    inline void UpdataChar() {
        int LastUnitXPos = 0;
        size_t Cnt = 0;
        for (auto Unit : Units) {
            if (Unit.X > LastUnitXPos) {
                LastUnitXPos = Unit.X;
                Cnt = Unit.EachBarData.size();
            }
        }
        this->X_Axis_Length = LastUnitXPos + 3 * BarWidth * this->X_Unit * Cnt;

        int LastUnitYPos = 0;
        for (auto Unit : Units) {
            for (auto Bar : Unit.EachBarData) {
                LastUnitYPos = max(Bar, LastUnitYPos);
            }
        }
        this->Y_Axis_Length = LastUnitYPos + 30 / this->Y_Unit;

        if (SampleRect.bottom == -1) {
            this->SampleRect.left = this->X_Axis_Length - 50 / this->X_Unit;
            this->SampleRect.top = this->Y_Axis_Length + 10 / this->Y_Unit;
        }

        for (auto Unit : Units) {
            for (int i = 0; i < Unit.EachBarData.size(); i++) {
                auto it = find_if(Samples.begin(), Samples.end(), 
                    [Unit,i](pair<COLORREF,string> S) {return Unit.EachBarText[i] == S.second; });//查找是否有重复项已存在
                if (it == Samples.end()) {//若没有
                    Samples.emplace_back(pair<COLORREF, string>(Unit.EachBarColor[i], Unit.EachBarText[i]));//录入
                }
            }
        }
    }

    vector<UnitData> Units;
    int X_Axis_Length = 0;//坐标轴长度
    int X_Unit = 0;//单位
    int Y_Axis_Length = 0;
    int Y_Unit = 0;
    int BarWidth = 10;//bar宽
    string Y_Name = "";//坐标轴文本
    string X_Name = "";
    HFONT hFont_Axis = NULL;//所有文本的字体
    bool YNEnableSample = false;//是否绘制图例
    RECT SampleRect = { 0,0,0,0 };//图例的显示区域，仅使用left和top，left和top分别代表距离起始点的长和高
    vector<pair<COLORREF, string>> Samples;//所有图例的颜色和文本
};

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LPWSTR              str2LPWSTR(string);
string              LPWSTR2str(LPWSTR);
void                DrawBarChart(HDC, POINT, ChartData, COLORREF);


//字符串转 L长 P指针 W宽字符 STR字符串 (支持汉字)
LPWSTR str2LPWSTR(string str) {//将string转为LPWSTR (支持汉字)
    int size = (int)str.length();//获取当前字符串的长度
    int wLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);//计算目标字符串的长度
    wchar_t* buffer = new wchar_t[wLen + 1];//定义新字符串
    memset(buffer, 0, (wLen + 1) * sizeof(wchar_t));//初始化
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), size, (LPWSTR)buffer, wLen);//str -> wchar_ts
    return buffer;//wchar_t -> LPWSTR
}

//LPWSTR转str
string LPWSTR2str(LPWSTR lpwstr) {
    int size = WideCharToMultiByte(CP_UTF8, 0, lpwstr, lstrlen(lpwstr), NULL, 0, NULL, NULL);//获取当前字符串的长度
    char* lpstr = new char[size + 1];//定义新字符串
    WideCharToMultiByte(CP_UTF8, 0, lpwstr, lstrlen(lpwstr), lpstr, size, NULL, NULL);//wchar_t -> str
    lpstr[size] = '\0';//添加结尾
    return lpstr;
}

//@brief 所有单位均使用对话框单位
//@param hdc, StartPos, Data, Axis
void DrawBarChart(HDC hdc, POINT StartPos, ChartData Data, COLORREF Axis = RGB(0, 0, 0)) {
    const int BarWidth = Data.GetBarWidth();//获取Bar的宽度

    //将对话框模板转换为像素
    StartPos.x = MulDiv(StartPos.x, LOWORD(GetDialogBaseUnits()), 4);
    StartPos.y = MulDiv(StartPos.y, HIWORD(GetDialogBaseUnits()), 8);

    //移动至起始位置
    MoveToEx(hdc, StartPos.x, StartPos.y, NULL);

    HPEN AxisPen = CreatePen(PS_SOLID, 1, Axis);//创建并选择指定颜色的画笔
    SelectObject(hdc, AxisPen);

    //绘制坐标轴
    LineTo(hdc, StartPos.x + MulDiv(Data.GetXAxisLength(), LOWORD(GetDialogBaseUnits()), 4), StartPos.y);//X

    MoveToEx(hdc, StartPos.x, StartPos.y, NULL);
    LineTo(hdc, StartPos.x, StartPos.y - MulDiv(Data.GetYAxisLength(), HIWORD(GetDialogBaseUnits()), 8));//Y

    //坐标轴文本
    if (Data.GetAxisFont())//当字体已被设置时读取并应用字体
        SelectObject(hdc, Data.GetAxisFont());

    RECT TextBox;//文本框
    TextBox.left = StartPos.x + MulDiv(Data.GetXAxisLength(), LOWORD(GetDialogBaseUnits()), 4) - 100;
    TextBox.right = StartPos.x + MulDiv(Data.GetXAxisLength(), LOWORD(GetDialogBaseUnits()), 4);
    TextBox.top = StartPos.y + 5;
    TextBox.bottom = StartPos.y + 30;
    DrawText(hdc, str2LPWSTR(Data.GetXName()), -1, &TextBox, DT_RIGHT | DT_TOP | DT_SINGLELINE);//X轴文本

    TextBox.left = StartPos.x - 100;
    TextBox.right = StartPos.x - 5;
    TextBox.top = StartPos.y - MulDiv(Data.GetYAxisLength(), HIWORD(GetDialogBaseUnits()), 8);
    TextBox.bottom = StartPos.y - MulDiv(Data.GetYAxisLength(), HIWORD(GetDialogBaseUnits()), 8) + 20;
    DrawText(hdc, str2LPWSTR(Data.GetYName()), -1, &TextBox, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);//Y轴文本

    //绘制Bar
    for (int i = 0; i < Data.GetUnitsCount(); i++) {
        //获取
        UnitData Unit = Data.GetUnitData(i);//获取单个Unit的数据
        vector<int> Bars = Unit.GetBarData();//获取所有的Bar的数据
        //处理
        int BarCnt = Bars.size();//获取Bar的个数
        double Start_X = (double)BarCnt / 2;//计算起始点
        int ptr = 0;//Bar数据下标

        for (double j = -Start_X; j < Start_X; j++) {
            //计算坐标
            int X_Pos = Unit.GetXPos() / Data.GetXUnit() + BarWidth * j;//formula : 中心 / 单位 + 宽度 * 偏移量
            int X_Pos_Pixel = StartPos.x + MulDiv(X_Pos, LOWORD(GetDialogBaseUnits()), 4);//转换为像素

            int Y_Pos = Bars[ptr] / Data.GetYUnit();//formula : 数据 / 单位
            int Y_Pos_Pixel = StartPos.y - MulDiv(Y_Pos, HIWORD(GetDialogBaseUnits()), 8);//转换为像素

            //进行
            HPEN BarPen = CreatePen(PS_INSIDEFRAME, 1, Unit.GetBarColor(ptr));//获取当前Bar的颜色，如果存在
            HPEN OldPen = (HPEN)SelectObject(hdc, BarPen);
            DeleteObject(OldPen);

            HBRUSH BarBrush = CreateHatchBrush(HS_BDIAGONAL, Unit.GetBarColor(ptr));
            HBRUSH OldBrush = (HBRUSH)SelectObject(hdc, BarBrush);
            DeleteObject(OldBrush);

            RECT _Rectangle;
            _Rectangle.left = X_Pos_Pixel;
            _Rectangle.right = X_Pos_Pixel + MulDiv(BarWidth, LOWORD(GetDialogBaseUnits()), 4);
            _Rectangle.top = Y_Pos_Pixel;
            _Rectangle.bottom = StartPos.y;

            Rectangle(hdc, _Rectangle.left, _Rectangle.top, _Rectangle.right, _Rectangle.bottom);

            RECT TextBox;
            TextBox.left = _Rectangle.left + 1, TextBox.right = _Rectangle.right;
            TextBox.top = _Rectangle.top - 20, TextBox.bottom = _Rectangle.top;
            DrawText(hdc, str2LPWSTR(to_string(Bars[ptr])), -1, &TextBox, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Bar文本

            ptr++;//Bar数据下标
        }

        RECT TextBox;
        TextBox.left = Unit.GetXPos() / Data.GetXUnit() - BarWidth;
        TextBox.right = Unit.GetXPos() / Data.GetXUnit() + BarWidth;
        TextBox.top = StartPos.y + 5, TextBox.bottom = StartPos.y + 30;

        TextBox.left = StartPos.x + MulDiv(TextBox.left, LOWORD(GetDialogBaseUnits()), 4);
        TextBox.right = StartPos.x + MulDiv(TextBox.right, LOWORD(GetDialogBaseUnits()), 4);
        DrawText(hdc, str2LPWSTR(Unit.GetText()), -1, &TextBox, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Unit文本
    }

    //绘制图例
    if (!Data.IsSampleEnable()) return;//判断是否需要绘制图例
    RECT Rect = Data.GetSampleRect();
    Rect.left = MulDiv(Rect.left, LOWORD(GetDialogBaseUnits()), 4);
    Rect.top = MulDiv(Rect.top, HIWORD(GetDialogBaseUnits()), 8);
    Rect.left += StartPos.x;
    Rect.top = StartPos.y - Rect.top;

    const int SampleLength = (double)BarWidth / 2;

    int Cnt = 0;//已处理的个数
    for (auto Sample : Data.GetSamples()) {
        HPEN BarPen = CreatePen(PS_INSIDEFRAME, 1, Sample.first);//获取当前Sample的颜色，如果存在
        HPEN OldPen = (HPEN)SelectObject(hdc, BarPen);
        DeleteObject(OldPen);

        HBRUSH BarBrush = CreateHatchBrush(HS_BDIAGONAL, Sample.first);
        HBRUSH OldBrush = (HBRUSH)SelectObject(hdc, BarBrush);
        DeleteObject(OldBrush);

        RECT SingleRect;
        SingleRect.left = Rect.left;
        SingleRect.top = Rect.top + Cnt * MulDiv(SampleLength * 2, HIWORD(GetDialogBaseUnits()), 8);
        SingleRect.right = Rect.left + MulDiv(SampleLength, LOWORD(GetDialogBaseUnits()), 4);
        SingleRect.bottom = SingleRect.top + MulDiv(SampleLength, HIWORD(GetDialogBaseUnits()), 8);

        Rectangle(hdc, SingleRect.left, SingleRect.top, SingleRect.right, SingleRect.bottom);

        RECT TextBox;
        TextBox.left = SingleRect.right + 20, TextBox.right = SingleRect.right + 128;
        TextBox.top = SingleRect.top, TextBox.bottom = SingleRect.top + 20;
        DrawText(hdc, str2LPWSTR(Sample.second), -1, &TextBox, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//Sample文本

        Cnt++;
    }
}

/*MessageBoxA(NULL,
            (to_string(SingleRect.left) + "\n" + to_string(SingleRect.right) + "\n" +
                to_string(SingleRect.top) + "\n" + to_string(SingleRect.bottom)).c_str(), "test", MB_OK
        );*/

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BARCHART, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BARCHART));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BARCHART));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BARCHART);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...

            ChartData Chart;//创建表格
            UnitData Unit;//创建Unit
            
            //插入Bar
            Unit.InsertBar(200, "Name1", RGB(255, 0, 0));
            Unit.InsertBar(100, "Name2", RGB(0, 255, 0));
            Unit.InsertBar(50, "Name3", RGB(0, 0, 255));
           
            //设置X坐标
            Unit.SetXPos(100);
            
            //设置Unit文本
            Unit.SetText("Item1");
            
            //创建字体
            HFONT hFont = CreateFont(0, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");

            //初始化表格
            //Chart.InitializeChart(vector<UnitData>(), 1, 1, "项目", "分数", 20, hFont, false);
            //Chart.InitializeChart(vector<UnitData>(), 1, 1, "项目", "分数", 20, hFont, true, RECT{ 200,200,0,0 });
            Chart.InitializeChart(vector<UnitData>(), 1, 1, "项目", "分数", 20, hFont);

            //插入Unit
            Chart.InsertUnit(Unit);

            //清空Unit
            Unit.clear();

            //同上
            Unit.InsertBar(100, "Name1", RGB(255, 0, 0));
            Unit.InsertBar(50, "Name2", RGB(0, 255, 0));
            Unit.InsertBar(60, "Name3", RGB(0, 0, 255));
            Unit.SetXPos(200);
            Unit.SetText("Item2");

            //同上
            Chart.InsertUnit(Unit);

            //设置起始点
            POINT StartPoint;
            StartPoint.x = 30;
            StartPoint.y = 250;
            //传入数据，绘制表格
            DrawBarChart(hdc, StartPoint, Chart);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
