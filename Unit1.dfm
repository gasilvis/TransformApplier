object Form1: TForm1
  Left = 171
  Top = 29
  Width = 1141
  Height = 839
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = 'TA, the AAVSO Transform Applier application'
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -18
  Font.Name = 'Arial'
  Font.Style = [fsBold]
  Menu = MainMenu1
  OldCreateOrder = False
  Scaled = False
  ShowHint = True
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 21
  object CoefPage: TPageControl
    Left = 0
    Top = 0
    Width = 1133
    Height = 781
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Application'
      object Label6: TLabel
        Left = 280
        Top = 8
        Width = 419
        Height = 21
        Caption = 'Paste your un-transformed AAVSO records here'
      end
      object Label7: TLabel
        Left = 280
        Top = 336
        Width = 575
        Height = 21
        Caption = 
          'The transformed records are here after you hit the Process butto' +
          'n'
      end
      object versionLabel: TLabel
        Left = 752
        Top = 8
        Width = 5
        Height = 21
        OnClick = versionLabelClick
      end
      object Memo2: TMemo
        Left = 152
        Top = 368
        Width = 953
        Height = 361
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ScrollBars = ssVertical
        TabOrder = 0
        WordWrap = False
      end
      object Memo1: TMemo
        Left = 152
        Top = 40
        Width = 953
        Height = 281
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ScrollBars = ssVertical
        TabOrder = 1
        WordWrap = False
      end
      object ProcessButton: TButton
        Left = 24
        Top = 416
        Width = 105
        Height = 25
        Hint = 'Transform  the untransformed'
        Caption = 'Process'
        TabOrder = 2
        OnClick = ProcessButtonClick
      end
      object ToClipboardButton: TButton
        Left = 8
        Top = 496
        Width = 137
        Height = 25
        Hint = 'copy the transformed observations to the clipboard'
        Caption = 'To Clipboard'
        TabOrder = 3
        OnClick = ToClipboardButtonClick
      end
      object RadioGroup1: TRadioGroup
        Left = 8
        Top = 48
        Width = 137
        Height = 201
        Caption = 'Methodology'
        TabOrder = 4
      end
      object RadioButton1: TRadioButton
        Left = 24
        Top = 136
        Width = 97
        Height = 17
        Hint = 
          'These transforms are gleaned from the literature; some are not r' +
          'ecommended'
        Caption = 'Classic'
        TabOrder = 5
      end
      object RadioButton2: TRadioButton
        Left = 24
        Top = 72
        Width = 97
        Height = 17
        Hint = 'These transforms are recommended by the AAVSO'
        Caption = 'AAVSO'
        Checked = True
        TabOrder = 6
        TabStop = True
      end
      object RadioButton3: TRadioButton
        Left = 24
        Top = 104
        Width = 113
        Height = 17
        Hint = 'These transforms are commonly used'
        Caption = 'Alternate'
        TabOrder = 7
      end
      object IncludeRaw: TCheckBox
        Left = 8
        Top = 392
        Width = 137
        Height = 17
        Hint = 'Include Un transformed obs with Transformed  '
        Caption = 'include raw'
        Enabled = False
        TabOrder = 8
        Visible = False
      end
      object Button4: TButton
        Left = 176
        Top = 8
        Width = 75
        Height = 25
        Caption = 'Clear'
        TabOrder = 9
        OnClick = Button4Click
      end
      object saveButton: TButton
        Left = 24
        Top = 560
        Width = 99
        Height = 25
        Caption = 'Save'
        TabOrder = 10
        OnClick = Save1Click
      end
      object UseStdField: TCheckBox
        Left = 8
        Top = 680
        Width = 145
        Height = 17
        Hint = 'Chart uses Standard Field photometry'
        Caption = 'UseStdField'
        TabOrder = 11
        OnClick = UseStdFieldClick
      end
      object AggregateCB: TCheckBox
        Left = 8
        Top = 360
        Width = 137
        Height = 25
        Hint = 'Aggregate all common obs'
        Caption = 'Aggregate'
        TabOrder = 12
      end
      object TestTCCB: TCheckBox
        Left = 8
        Top = 336
        Width = 129
        Height = 17
        Hint = 
          'Run transforms on K stars to see if you get back to the std valu' +
          'es.'
        Caption = 'Test TC'
        TabOrder = 13
      end
    end
    object Report: TTabSheet
      Caption = 'Report'
      ImageIndex = 1
      object Memo4: TMemo
        Left = 32
        Top = 24
        Width = 1065
        Height = 705
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -17
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
      object Button2: TButton
        Left = 816
        Top = 0
        Width = 75
        Height = 25
        Caption = 'Button2'
        TabOrder = 1
        Visible = False
        OnClick = Button2Click
      end
      object Button3: TButton
        Left = 48
        Top = 0
        Width = 75
        Height = 25
        Caption = 'Clear'
        TabOrder = 2
        OnClick = Button3Click
      end
      object CopyReportButton: TButton
        Left = 152
        Top = 0
        Width = 137
        Height = 25
        Hint = 'copy the transformed observations to the clipboard'
        Caption = 'To Clipboard'
        TabOrder = 3
        OnClick = CopyReportButtonClick
      end
    end
    object Coefficients: TTabSheet
      Caption = 'Coefficients'
      ImageIndex = 2
      object Label17: TLabel
        Left = 40
        Top = 8
        Width = 56
        Height = 21
        Hint = 'Describe the setup for the coefficiets documented below'
        Caption = 'setup:'
      end
      object GroupBox1: TGroupBox
        Left = 24
        Top = 32
        Width = 1073
        Height = 705
        TabOrder = 0
        object Label19: TLabel
          Left = 712
          Top = 16
          Width = 43
          Height = 21
          Caption = 'error'
        end
        object Label20: TLabel
          Left = 544
          Top = 16
          Width = 95
          Height = 21
          Caption = 'Coefficient'
        end
        object Label2: TCheckBox
          Left = 24
          Top = 174
          Width = 473
          Height = 20
          Hint = 'reciprocal of slope of (v-r) versus (V-R)'
          Caption = 'Tvr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 25
          OnClick = CheckBox4Click
        end
        object Label3: TCheckBox
          Left = 24
          Top = 238
          Width = 473
          Height = 20
          Hint = 'reciprocal of slope of (r-i) versus (R-I)'
          Caption = 'Tri'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 26
          OnClick = CheckBox4Click
        end
        object Label4: TCheckBox
          Left = 24
          Top = 414
          Width = 473
          Height = 21
          Hint = 'slope of (V-v) versus (V-R)'
          Caption = 'Tv_vr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 27
          OnClick = CheckBox4Click
        end
        object Label5: TCheckBox
          Left = 24
          Top = 478
          Width = 473
          Height = 21
          Hint = 'slope of (R-r) versus (R-I)'
          Caption = 'Tr_ri'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 28
          OnClick = CheckBox4Click
        end
        object Label9: TCheckBox
          Left = 24
          Top = 318
          Width = 465
          Height = 21
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb_bv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 29
          OnClick = CheckBox4Click
        end
        object Label10: TCheckBox
          Left = 24
          Top = 206
          Width = 473
          Height = 20
          Hint = 'reciprocal of slope of (v-i) versus (V-I)'
          Caption = 'Tvi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 30
          OnClick = CheckBox4Click
        end
        object Label8: TCheckBox
          Left = 24
          Top = 446
          Width = 465
          Height = 21
          Hint = 'slope of (V-v) versus (V-I)'
          Caption = 'Tv_vi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 31
          OnClick = CheckBox4Click
        end
        object Label12: TCheckBox
          Left = 24
          Top = 142
          Width = 473
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tbi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 32
          OnClick = CheckBox4Click
        end
        object Label13: TCheckBox
          Left = 24
          Top = 350
          Width = 473
          Height = 21
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb_br'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 33
          OnClick = CheckBox4Click
        end
        object Label14: TCheckBox
          Left = 24
          Top = 382
          Width = 473
          Height = 21
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb_bi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 34
          OnClick = CheckBox4Click
        end
        object Label11: TCheckBox
          Left = 24
          Top = 110
          Width = 473
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tbr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 35
          OnClick = CheckBox4Click
        end
        object Label1: TCheckBox
          Left = 24
          Top = 78
          Width = 457
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tbv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 36
          OnClick = CheckBox4Click
        end
        object TbvEdit: TEdit
          Left = 520
          Top = 72
          Width = 121
          Height = 29
          TabOrder = 0
          OnExit = NumericOnExit
        end
        object TvrEdit: TEdit
          Left = 520
          Top = 168
          Width = 121
          Height = 29
          TabOrder = 3
          OnExit = NumericOnExit
        end
        object TriEdit: TEdit
          Left = 520
          Top = 232
          Width = 121
          Height = 29
          TabOrder = 5
          OnExit = NumericOnExit
        end
        object Tv_vrEdit: TEdit
          Left = 520
          Top = 408
          Width = 121
          Height = 29
          TabOrder = 9
          OnExit = NumericOnExit
        end
        object Tr_riEdit: TEdit
          Left = 520
          Top = 472
          Width = 121
          Height = 29
          TabOrder = 11
          OnExit = NumericOnExit
        end
        object Tb_bvEdit: TEdit
          Left = 520
          Top = 312
          Width = 121
          Height = 29
          TabOrder = 6
          OnExit = NumericOnExit
        end
        object TviEdit: TEdit
          Left = 520
          Top = 200
          Width = 121
          Height = 29
          TabOrder = 4
          OnExit = NumericOnExit
        end
        object Tv_viEdit: TEdit
          Left = 520
          Top = 440
          Width = 121
          Height = 29
          TabOrder = 10
          OnExit = NumericOnExit
        end
        object TbrEdit: TEdit
          Left = 520
          Top = 104
          Width = 121
          Height = 29
          TabOrder = 1
          OnExit = NumericOnExit
        end
        object TbiEdit: TEdit
          Left = 520
          Top = 136
          Width = 121
          Height = 29
          TabOrder = 2
          OnExit = NumericOnExit
        end
        object Tb_brEdit: TEdit
          Left = 520
          Top = 344
          Width = 121
          Height = 29
          TabOrder = 7
          OnExit = NumericOnExit
        end
        object Tb_biEdit: TEdit
          Left = 520
          Top = 376
          Width = 121
          Height = 29
          TabOrder = 8
          OnExit = NumericOnExit
        end
        object rTr_riEdit: TEdit
          Left = 680
          Top = 472
          Width = 121
          Height = 29
          TabOrder = 24
          OnExit = NumericOnExit
        end
        object rTv_viEdit: TEdit
          Left = 680
          Top = 440
          Width = 121
          Height = 29
          TabOrder = 23
          OnExit = NumericOnExit
        end
        object rTv_vrEdit: TEdit
          Left = 680
          Top = 408
          Width = 121
          Height = 29
          TabOrder = 22
          OnExit = NumericOnExit
        end
        object rTb_biEdit: TEdit
          Left = 680
          Top = 376
          Width = 121
          Height = 29
          TabOrder = 21
          OnExit = NumericOnExit
        end
        object rTb_brEdit: TEdit
          Left = 680
          Top = 344
          Width = 121
          Height = 29
          TabOrder = 20
          OnExit = NumericOnExit
        end
        object rTb_bvEdit: TEdit
          Left = 680
          Top = 312
          Width = 121
          Height = 29
          TabOrder = 19
          OnExit = NumericOnExit
        end
        object rTriEdit: TEdit
          Left = 680
          Top = 232
          Width = 121
          Height = 29
          TabOrder = 18
          OnExit = NumericOnExit
        end
        object rTviEdit: TEdit
          Left = 680
          Top = 200
          Width = 121
          Height = 29
          TabOrder = 17
          OnExit = NumericOnExit
        end
        object rTvrEdit: TEdit
          Left = 680
          Top = 168
          Width = 121
          Height = 29
          TabOrder = 16
          OnExit = NumericOnExit
        end
        object rTbiEdit: TEdit
          Left = 680
          Top = 136
          Width = 121
          Height = 29
          TabOrder = 15
          OnExit = NumericOnExit
        end
        object rTbrEdit: TEdit
          Left = 680
          Top = 104
          Width = 121
          Height = 29
          TabOrder = 14
          OnExit = NumericOnExit
        end
        object rTbvEdit: TEdit
          Left = 680
          Top = 72
          Width = 121
          Height = 29
          TabOrder = 13
          OnExit = NumericOnExit
        end
        object CheckBox1: TCheckBox
          Left = 24
          Top = 510
          Width = 465
          Height = 21
          Hint = 'slope of (V-v) versus (V-R)'
          Caption = 'Tv_bv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 37
          OnClick = CheckBox4Click
        end
        object CheckBox2: TCheckBox
          Left = 24
          Top = 542
          Width = 473
          Height = 21
          Hint = 'slope of (V-v) versus (V-I)'
          Caption = 'Tr_bv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 38
          OnClick = CheckBox4Click
        end
        object CheckBox3: TCheckBox
          Left = 24
          Top = 574
          Width = 473
          Height = 21
          Hint = 'slope of (R-r) versus (R-I)'
          Caption = 'Ti_bv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 39
          OnClick = CheckBox4Click
        end
        object Tv_bvEdit: TEdit
          Left = 520
          Top = 504
          Width = 121
          Height = 29
          TabOrder = 12
          OnExit = NumericOnExit
        end
        object rTv_bvEdit: TEdit
          Left = 680
          Top = 504
          Width = 121
          Height = 29
          TabOrder = 40
          OnExit = NumericOnExit
        end
        object rTr_bvEdit: TEdit
          Left = 680
          Top = 536
          Width = 121
          Height = 29
          TabOrder = 41
          OnExit = NumericOnExit
        end
        object rTi_bvEdit: TEdit
          Left = 680
          Top = 568
          Width = 121
          Height = 29
          TabOrder = 42
          OnExit = NumericOnExit
        end
        object Ti_bvEdit: TEdit
          Left = 520
          Top = 568
          Width = 121
          Height = 29
          TabOrder = 43
          OnExit = NumericOnExit
        end
        object Tr_bvEdit: TEdit
          Left = 520
          Top = 536
          Width = 121
          Height = 29
          TabOrder = 44
          OnExit = NumericOnExit
        end
        object CheckBox9: TCheckBox
          Left = 24
          Top = 606
          Width = 465
          Height = 21
          Hint = 'slope of (V-v) versus (V-R)'
          Caption = 'Tr_vr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 45
          OnClick = CheckBox4Click
        end
        object CheckBox10: TCheckBox
          Left = 24
          Top = 638
          Width = 473
          Height = 21
          Hint = 'slope of (V-v) versus (V-I)'
          Caption = 'Tr_vi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 46
          OnClick = CheckBox4Click
        end
        object CheckBox11: TCheckBox
          Left = 24
          Top = 670
          Width = 473
          Height = 21
          Hint = 'slope of (R-r) versus (R-I)'
          Caption = 'Ti_vi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 47
          OnClick = CheckBox4Click
        end
        object Tr_vrEdit: TEdit
          Left = 520
          Top = 600
          Width = 121
          Height = 29
          TabOrder = 48
          OnExit = NumericOnExit
        end
        object rTr_vrEdit: TEdit
          Left = 680
          Top = 600
          Width = 121
          Height = 29
          TabOrder = 49
          OnExit = NumericOnExit
        end
        object rTr_viEdit: TEdit
          Left = 680
          Top = 632
          Width = 121
          Height = 29
          TabOrder = 50
          OnExit = NumericOnExit
        end
        object rTi_viEdit: TEdit
          Left = 680
          Top = 664
          Width = 121
          Height = 29
          TabOrder = 51
          OnExit = NumericOnExit
        end
        object Ti_viEdit: TEdit
          Left = 520
          Top = 664
          Width = 121
          Height = 29
          TabOrder = 52
          OnExit = NumericOnExit
        end
        object Tr_viEdit: TEdit
          Left = 520
          Top = 632
          Width = 121
          Height = 29
          TabOrder = 53
          OnExit = NumericOnExit
        end
        object CheckBox12: TCheckBox
          Left = 24
          Top = 286
          Width = 465
          Height = 21
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tu_ub'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 54
          OnClick = CheckBox4Click
        end
        object Tu_ubEdit: TEdit
          Left = 520
          Top = 280
          Width = 121
          Height = 29
          TabOrder = 55
          OnExit = NumericOnExit
        end
        object rTu_ubEdit: TEdit
          Left = 680
          Top = 280
          Width = 121
          Height = 29
          TabOrder = 56
          OnExit = NumericOnExit
        end
        object CheckBox13: TCheckBox
          Left = 24
          Top = 46
          Width = 473
          Height = 20
          Hint = 'reciprocal of slope of (r-i) versus (R-I)'
          Caption = 'Tub'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 57
          OnClick = CheckBox4Click
        end
        object TubEdit: TEdit
          Left = 520
          Top = 40
          Width = 121
          Height = 29
          TabOrder = 58
          OnExit = NumericOnExit
        end
        object rTubEdit: TEdit
          Left = 680
          Top = 40
          Width = 121
          Height = 29
          TabOrder = 59
          OnExit = NumericOnExit
        end
        object Panel1: TPanel
          Left = 8
          Top = 32
          Width = 33
          Height = 665
          BevelOuter = bvNone
          TabOrder = 60
        end
      end
      object setupEdit: TEdit
        Left = 112
        Top = 5
        Width = 745
        Height = 29
        Hint = 'Describe the setup for the coefficiets documented below'
        TabOrder = 1
        OnExit = setupEditExit
      end
      object changeINIfile: TButton
        Left = 872
        Top = 8
        Width = 75
        Height = 25
        Hint = 'Change the ini (setup info) that you want opened'
        Caption = 'change'
        TabOrder = 2
        OnClick = changeINIfileClick
      end
      object saveasINIfile: TButton
        Left = 960
        Top = 8
        Width = 75
        Height = 25
        Hint = 'Save this setup informamtion in a different ini file'
        Caption = 'save as'
        TabOrder = 3
        OnClick = saveasINIfileClick
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Extinction'
      ImageIndex = 4
      object GroupBox2: TGroupBox
        Left = 16
        Top = 16
        Width = 385
        Height = 233
        Caption = 'Coefficients'
        TabOrder = 0
        object Label23: TLabel
          Left = 264
          Top = 32
          Width = 43
          Height = 21
          Caption = 'error'
        end
        object Label15: TLabel
          Left = 136
          Top = 32
          Width = 48
          Height = 21
          Caption = 'value'
        end
        object rEuEdit: TEdit
          Left = 240
          Top = 184
          Width = 121
          Height = 29
          TabOrder = 0
          OnExit = NumericOnExit
        end
        object rEiEdit: TEdit
          Left = 240
          Top = 152
          Width = 121
          Height = 29
          TabOrder = 1
          OnExit = NumericOnExit
        end
        object rErEdit: TEdit
          Left = 240
          Top = 120
          Width = 121
          Height = 29
          TabOrder = 2
          OnExit = NumericOnExit
        end
        object rEvEdit: TEdit
          Left = 240
          Top = 88
          Width = 121
          Height = 29
          TabOrder = 3
          OnExit = NumericOnExit
        end
        object rEbEdit: TEdit
          Left = 240
          Top = 56
          Width = 121
          Height = 29
          TabOrder = 4
          OnExit = NumericOnExit
        end
        object EuEdit: TEdit
          Left = 104
          Top = 184
          Width = 121
          Height = 29
          TabOrder = 5
          OnExit = NumericOnExit
        end
        object EiEdit: TEdit
          Left = 104
          Top = 152
          Width = 121
          Height = 29
          TabOrder = 6
          OnExit = NumericOnExit
        end
        object ErEdit: TEdit
          Left = 104
          Top = 120
          Width = 121
          Height = 29
          TabOrder = 7
          OnExit = NumericOnExit
        end
        object EvEdit: TEdit
          Left = 104
          Top = 88
          Width = 121
          Height = 29
          TabOrder = 8
          OnExit = NumericOnExit
        end
        object EbEdit: TEdit
          Left = 104
          Top = 56
          Width = 121
          Height = 29
          TabOrder = 9
          OnExit = NumericOnExit
        end
        object applyExtinction: TCheckBox
          Left = 8
          Top = 32
          Width = 81
          Height = 17
          Hint = 
            'Extinction is turned off for this version of TA. Read the Help t' +
            'ab.'
          Caption = 'apply'
          TabOrder = 10
          OnExit = applyExtinctionExit
        end
        object CheckBox4: TCheckBox
          Left = 32
          Top = 62
          Width = 50
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'B'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 11
          OnClick = CheckBox4Click
        end
        object CheckBox5: TCheckBox
          Left = 32
          Top = 94
          Width = 50
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'V'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 12
          OnClick = CheckBox4Click
        end
        object CheckBox6: TCheckBox
          Left = 32
          Top = 126
          Width = 50
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'R'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 13
          OnClick = CheckBox4Click
        end
        object CheckBox7: TCheckBox
          Left = 32
          Top = 158
          Width = 50
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'I'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 14
          OnClick = CheckBox4Click
        end
        object CheckBox8: TCheckBox
          Left = 32
          Top = 190
          Width = 50
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'U'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 15
          OnClick = CheckBox4Click
        end
        object Panel2: TPanel
          Left = 24
          Top = 56
          Width = 25
          Height = 169
          BevelOuter = bvNone
          TabOrder = 16
        end
      end
      object Button5: TButton
        Left = 544
        Top = 72
        Width = 89
        Height = 25
        Caption = 'Button5'
        TabOrder = 1
        Visible = False
        OnClick = Button5Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Help'
      ImageIndex = 1
      object Memo3: TMemo
        Left = 16
        Top = 8
        Width = 1097
        Height = 721
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -18
        Font.Name = 'Courier New'
        Font.Style = [fsBold]
        Lines.Strings = (
          ''
          
            'Welcome to the Transform Applier (TA), a tool prepared for the A' +
            'AVSO to make it easy to submit '
          
            'transformed observations to WebObs. We, members of the AAVSO, wa' +
            'nt to submit transformed '
          
            'observations because these can then be used by professional astr' +
            'onomers. We'#39're amateurs, but '
          
            'sophistocated amateurs, and this tool will help us go the next s' +
            'tep.'
          ''
          'What is required to do this is:   '
          
            '   - your observations in AAVSO extended format, ready for WebOb' +
            's submission'
          
            '   - transform coefficients appropriate for your observing platf' +
            'orm'
          '   That'#39's it!'
          ''
          
            'Once you have set your coefficients into TA, you load your obser' +
            'vations into the program, hit'
          
            'the Process button and, voila, you have a transformed, AAVSO ext' +
            'ended format, file ready for'
          'submission.'
          ''
          
            'Please note: Ensemble observations are not transformable with th' +
            'is tool. There are ways'
          
            'to do transformed ensembles and TA may be an aid to this. But th' +
            'e process is not yet'
          'documented.'
          ''
          'The details:'
          ''
          
            '- This is a simple windows application that will run on WinXP or' +
            ' better. For you Mac or Linux'
          
            'users, you should be able to run it in a virtual Windows environ' +
            'ment or with Wine.'
          ''
          
            '- Your computer will need internet access so that TA can fetch s' +
            'tandard magnitudes for your comp'
          'stars.'
          ''
          
            '- Comp and check stars should be identified by their AUID'#39's. If ' +
            'you have used the chart labels,'
          
            'that might work too. The problem is that there are often duplica' +
            'te labels on a chart. TA will '
          
            'try to identify the labels and warn you if there is a duplicates' +
            ' problem. Worse comes to worse,'
          
            'you can insert the comp stars magnitude into the observation fil' +
            'e by adding a comment line'
          
            'before the first line that that refers to that comp that looks l' +
            'ike:'
          
            '"#CREFMAG= <Cmag> <Cerr> <Kmag> <Kerr>". Note that you can inclu' +
            'de the Check star data too. If '
          
            'you only need to provide the check star, you can use "#KREFMAG= ' +
            '<Kmag> <Kerr>"'
          ''
          '- Transform coefficients: a notation note'
          'Txy is a coefficient (called color or secondary) of the form:'
          
            '    1/ slope of (x-y) vs (X-Y)   of your standard star field dat' +
            'a.'
          
            '    where lower case variables are observed mags, capitalized ar' +
            'e standard mags.'
          '    eg.  Tbv is 1/ slope of (b-v) vs (B-V)'
          'Tx_yz is a coefficient (called star or primary) of the form:'
          '    slope of (X-x) vs (Y-Z)'
          '    eg.  Tb_bv is slope of (B-b) vs (B-V)'
          
            '(nb. the AAVSO is working on a tool to help you calculate your c' +
            'oefficients from a set of'
          'observations of a standard field like M67)'
          ''
          
            '- The AAVOS recommended transforms that the TA performs require ' +
            'the following coefficients:'
          'for      U       B       V       R       I'
          'UBVRI  Tu_ub   Tb_bv   Tv_bv   Tr_vi   Ti_vi'
          'UBVI   Tu_ub   Tb_bv   Tv_bv           Ti_vi'
          'UBV    Tu_ub   Tb_bv   Tv_bv'
          'BVRI           Tb_bv   Tv_bv   Tr_vi   Ti_vi'
          'BVR            Tb_bv   Tv_bv   Tvr'
          'BVI            Tb_bv   Tv_bv           Ti_vi'
          'VRI                    Tv_vi   Tr_vi   Tvi'
          'BV             Tb_bv   Tv_bv'
          'VR                     Tv_vr   Tr_vr'
          'VI                     Tv_vi           Tvi'
          
            'To transform your data you need to group your observations. Abov' +
            'e is the list of groupings that'
          
            'are supported and the coefficients that are required. If you hav' +
            'e not loaded these into the'
          'coefficients page, you will get a complaint from TA.'
          '- The Alternate transform equation set uses:'
          'for      U       B       V       R       I'
          'UBVRI   Tub    Tbv     Tv_bv   Tvr     Tri'
          'UBVR    Tub    Tb_bv   Tbv     Tvr'
          'UBVI    Tub    Tb_bv   Tbv             Tvi'
          'BVRI           Tbv     Tv_bv   Tvr     Tri'
          'BVR            Tb_bv   Tbv     Tvr'
          'BVI            Tb_bv   Tbv             Tvi'
          'VRI                    Tv_vr   Tvr     Tri'
          'BV             Tb_bv   Tbv'
          'VR                     Tv_vr   Tvr'
          'VI                     Tv_vi           Tvi'
          '- The classic transform equations from the literature are:'
          'for      U       B       V       R       I'
          'BVRI           Tr_ri, Tri, Tv_vr, Tvr and Tbv   deprecated'
          'BVR            Tbv     Tv_vr   Tvr'
          'BVI            Tbv     Tv_vi           Tvi'
          'VRI            Tr_ri, Tri, Tv_vr and Tvr        deprecated'
          'BV             Tb_bv   Tbv'
          'VR                     Tv_vr   Tvr'
          'VI                     Tv_vi           Tvi'
          
            'Classic BVRI and VRI are deprecated because they use too many co' +
            'efficients, one more than is'
          'needed. This reduces the value of the result.'
          ''
          
            '- Coefficient error values: Yes, you should submit the errors re' +
            'lated to your coefficients. The'
          
            'TA will propagate them into the final error of the observation. ' +
            'If you have used an Excel'
          
            'spreadsheet and the LINEST function, extract the error with (eg)' +
            ' =INDEX(LINEST'
          
            '(b_v,Bref_Vref,1,1),2,1) that is, the se, standard error, of the' +
            ' slope estimate.'
          'PTGP provides this error value for you.'
          ''
          
            '- Error computations: TA will compute a revised observation erro' +
            'r based on the VERR you '
          
            'submitted (which should be the instrumental errors of star and c' +
            'omp combined in quadrature)'
          
            'and the error of the transform coefficient. Note that the error ' +
            'of the comp reference '
          
            'magnitude is not included in the final error. This conforms to t' +
            'he WebObs/AID standard '
          'for reporting errors.'
          ''
          
            '- TA stores your constants in the transformer.ini file alongside' +
            ' the Transformer.exe file. If'
          
            'you have more than one set of coefficients, corresponding to a d' +
            'ifferent observing platform, you'
          
            'may have multiple INI files. Set the data on the coefficients pa' +
            'ge and execute "save as". You'
          
            'can change back to other ini files with the "change" button. TA ' +
            'will remember the last ini file'
          'that you accessed.'
          ''
          
            '- Extinction: Extinction is important, but this formulation of t' +
            'he transform process cancels out'
          
            'extinction because it assumes the target and comp star are at th' +
            'e same airmass. So, extinction'
          
            'is kept turned off. Future versions of TA will be able to adjust' +
            ' your data for extinction. This'
          
            'will depend on giving you a mechanism to specify the airmass of ' +
            'the comp star where it is'
          'different from the target.'
          ''
          
            '- Aggregating your data: If you have multiple observations of a ' +
            'variable star that is'
          
            'varying 100 times slower than the time span of your observations' +
            ', then you should aggregate'
          
            'your data; it will give you better error values. If you are obse' +
            'rving a fast variable then'
          
            'don'#39't aggregate. TA will aggregate your data for you if you chec' +
            'k the box. This is'
          
            'a simple aggregation with the Verr taken as the standard deviati' +
            'on of the mean. If you are'
          
            'looking for a more sophisticated aggregation, you should use the' +
            ' Boxter program.'
          ''
          
            '- Grouping: You should assign your observations into transform g' +
            'roups. If you do not, TA will'
          
            'attempt to group them for you. You should review the output to m' +
            'ake sure they were grouped in a'
          'reasonable fashion.'
          ''
          
            '- Use of the Std Field check box: If your chartid was generated ' +
            'with the "Would you like a '
          
            'Standard Field Chart", then check this box so TA will know how t' +
            'o fetch that data. This is '
          
            'an issue with VSP where the chartid is not uniquely defining the' +
            ' chart.'
          ''
          
            '- Test TC checkbox: This is an reality check tool. How good are ' +
            'your transforms? In'
          
            'theory you should be able to observe a known star and be able to' +
            ' reproduce its published'
          
            'magnitude. This test will move your K star data into the V posit' +
            'ion and then when you'
          
            'process it will transform that observation. The Report tab will ' +
            'then compare your results'
          
            'with the K stars standard magnitude. If your results don'#39't match' +
            ' within your error or if'
          
            'there is a systematic deviation, you should check your observati' +
            'on and assumptions'
          
            'because your system plus coefficients is not able to match stand' +
            'ard data.'
          ''
          ''
          
            '- If you have problems or issues or questions with or about the ' +
            'TA, you are welcome to contact'
          
            'the author, George Silvis, at SGEO@gasilvis.net. Or use the AAVS' +
            'O software development forum '
          'for these questions.'
          ''
          ''
          ''
          ''
          ''
          '')
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssVertical
        TabOrder = 0
      end
    end
    object AnalysisTabSheet: TTabSheet
      ImageIndex = 5
      OnEnter = AnalysisTabSheetEnter
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 1125
        Height = 745
        Align = alClient
        Alignment = taLeftJustify
        TabOrder = 0
        Visible = False
        object Label18: TLabel
          Left = 40
          Top = 80
          Width = 68
          Height = 21
          Caption = 'Label18'
        end
        object ListBox1: TListBox
          Left = 104
          Top = 24
          Width = 337
          Height = 33
          ItemHeight = 21
          TabOrder = 0
        end
      end
    end
  end
  object Button1: TButton
    Left = 1016
    Top = 640
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 1
    Visible = False
    OnClick = Button1Click
  end
  object MainMenu1: TMainMenu
    Left = 284
    Top = 422
    object File1: TMenuItem
      Caption = '&File'
      object Open1: TMenuItem
        Caption = '&Open un-transformed obs file'
        Hint = 'Open un-transformed observation file'
        OnClick = Open1Click
      end
      object Saveuntransformedobsfile1: TMenuItem
        Caption = 'Save &un-transformed obs file'
        OnClick = Saveuntransformedobsfile1Click
      end
      object Save1: TMenuItem
        Caption = '&Save transfomed obs to file'
        Hint = 'Save the transformed observations'
        OnClick = Save1Click
      end
      object SaveReporttofile1: TMenuItem
        Caption = 'Save &Report to file'
        OnClick = SaveReporttofile1Click
      end
      object Exit1: TMenuItem
        Caption = 'E&xit'
        OnClick = Exit1Click
      end
    end
  end
  object ElasticForm1: TElasticForm
    DesignScreenWidth = 1448
    DesignScreenHeight = 908
    DesignPixelsPerInch = 120
    DesignFormWidth = 1141
    DesignFormHeight = 839
    DesignFormClientWidth = 1133
    DesignFormClientHeight = 781
    DesignFormLeft = 171
    DesignFormTop = 29
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    Version = 700
    Left = 308
    Top = 542
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'txt'
    Filter = 'Text files (*.txt)|*.TXT'
    Title = 'Transformed observations'
    Left = 364
    Top = 406
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'txt'
    Filter = 'Text files (*.txt)|*.TXT'
    Title = 'Un-transformed observations'
    Left = 340
    Top = 470
  end
  object HttpCli1: THttpCli
    LocalAddr = '0.0.0.0'
    ProxyPort = '80'
    Agent = 'Mozilla/4.0'
    Accept = 'image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*'
    NoCache = False
    ContentTypePost = 'application/x-www-form-urlencoded'
    MultiThreaded = False
    RequestVer = '1.0'
    FollowRelocation = True
    LocationChangeMaxCount = 5
    BandwidthLimit = 10000
    BandwidthSampling = 1000
    Options = []
    OnDocBegin = HttpCli1DocBegin
    OnDocEnd = HttpCli1DocEnd
    SocksAuthentication = socksNoAuthentication
    Left = 188
    Top = 574
  end
end
