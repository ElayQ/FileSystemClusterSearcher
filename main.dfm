object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 271
  ClientWidth = 800
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 57
    Top = 28
    Width = 126
    Height = 16
    Caption = 'Enter volume letter'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 472
    Top = 222
    Width = 101
    Height = 13
    Caption = 'Clusters read count: '
    Visible = False
  end
  object Label4: TLabel
    Left = 472
    Top = 203
    Width = 120
    Height = 13
    Caption = 'Found signatures count: '
    Visible = False
  end
  object Label5: TLabel
    Left = 598
    Top = 222
    Width = 46
    Height = 13
    Visible = False
  end
  object Label6: TLabel
    Left = 598
    Top = 203
    Width = 46
    Height = 13
    Visible = False
  end
  object Label13: TLabel
    Left = 57
    Top = 122
    Width = 131
    Height = 13
    Caption = 'FileSystem information'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object Label9: TLabel
    Left = 57
    Top = 160
    Width = 62
    Height = 13
    Caption = 'Cluster size: '
    Visible = False
  end
  object Label11: TLabel
    Left = 57
    Top = 198
    Width = 79
    Height = 13
    Caption = 'FileSystem size: '
    Visible = False
  end
  object Label10: TLabel
    Left = 57
    Top = 179
    Width = 71
    Height = 13
    Caption = 'Cluster count: '
    Visible = False
  end
  object Label8: TLabel
    Left = 57
    Top = 141
    Width = 58
    Height = 13
    Caption = 'FileSystem: '
    Visible = False
  end
  object Label2: TLabel
    Left = 472
    Top = 243
    Width = 120
    Height = 15
    Alignment = taCenter
    Caption = 'Overall reading progress'
    Transparent = False
    Visible = False
    WordWrap = True
  end
  object Label7: TLabel
    Left = 755
    Top = 243
    Width = 32
    Height = 13
    Caption = 'Done!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object GroupBox1: TGroupBox
    Left = 263
    Top = 28
    Width = 153
    Height = 140
    Caption = 'Select signatures to search'
    Color = cl3DLight
    ParentBackground = False
    ParentColor = False
    TabOrder = 7
    object CheckBox1: TCheckBox
      Left = 56
      Top = 24
      Width = 41
      Height = 17
      Caption = 'ALL'
      Enabled = False
      TabOrder = 0
      OnClick = CheckBox1Click
    end
    object CheckBox2: TCheckBox
      Left = 16
      Top = 50
      Width = 41
      Height = 17
      Caption = 'Png'
      Enabled = False
      TabOrder = 1
    end
    object CheckBox3: TCheckBox
      Left = 16
      Top = 80
      Width = 41
      Height = 17
      Caption = 'JPG'
      Enabled = False
      TabOrder = 2
    end
    object CheckBox4: TCheckBox
      Left = 16
      Top = 110
      Width = 41
      Height = 17
      Caption = 'BMP'
      Enabled = False
      TabOrder = 3
    end
    object CheckBox5: TCheckBox
      Left = 96
      Top = 50
      Width = 41
      Height = 17
      Caption = 'GIF'
      Enabled = False
      TabOrder = 4
    end
    object CheckBox6: TCheckBox
      Left = 96
      Top = 80
      Width = 41
      Height = 17
      Caption = 'PDF'
      Enabled = False
      TabOrder = 5
    end
    object CheckBox7: TCheckBox
      Left = 96
      Top = 110
      Width = 41
      Height = 17
      Caption = 'EXE'
      Enabled = False
      TabOrder = 6
    end
  end
  object Button1: TButton
    Left = 247
    Top = 174
    Width = 75
    Height = 25
    Caption = 'Start iterator'
    Enabled = False
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 353
    Top = 174
    Width = 81
    Height = 25
    Caption = 'Start decorator'
    Enabled = False
    TabOrder = 1
    OnClick = Button2Click
  end
  object Edit1: TEdit
    Left = 62
    Top = 50
    Width = 121
    Height = 21
    TabOrder = 2
    OnKeyPress = Edit1KeyPress
    OnMouseLeave = Edit1MouseLeave
  end
  object Button3: TButton
    Left = 300
    Top = 205
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 3
    OnClick = Button3Click
  end
  object VirtualStringTree1: TVirtualStringTree
    Left = 472
    Top = 28
    Width = 273
    Height = 164
    AccessibleName = 'Signature'
    Enabled = False
    Header.AutoSizeIndex = -1
    Header.Height = 20
    Header.MainColumn = 1
    Header.Options = [hoColumnResize, hoDrag, hoShowSortGlyphs, hoVisible]
    TabOrder = 4
    TreeOptions.SelectionOptions = [toFullRowSelect, toMultiSelect]
    OnGetText = VirtualStringTree1GetText
    Touch.InteractiveGestures = [igPan, igPressAndTap]
    Touch.InteractiveGestureOptions = [igoPanSingleFingerHorizontal, igoPanSingleFingerVertical, igoPanInertia, igoPanGutter, igoParentPassthrough]
    Columns = <
      item
        Position = 0
        Text = 'Cluster Number'
        Width = 179
      end
      item
        Position = 1
        Text = 'Signature'
        Width = 87
      end>
  end
  object Button4: TButton
    Left = 751
    Top = 44
    Width = 37
    Height = 48
    Caption = '-'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
    Visible = False
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 751
    Top = 124
    Width = 37
    Height = 44
    Caption = 'x'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 6
    Visible = False
    OnClick = Button5Click
  end
  object ProgressBar1: TProgressBar
    Left = 598
    Top = 241
    Width = 150
    Height = 17
    Smooth = True
    TabOrder = 8
    Visible = False
  end
  object Button6: TButton
    Left = 80
    Top = 80
    Width = 85
    Height = 25
    Caption = 'Open disk'
    Enabled = False
    TabOrder = 9
    OnClick = Button6Click
  end
end
