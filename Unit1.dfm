object Form1: TForm1
  Left = 309
  Top = 149
  Width = 1024
  Height = 719
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = 'Coefficient Transformer'
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = 'Arial'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Scaled = False
  ShowHint = True
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 19
  object CoefPage: TPageControl
    Left = 0
    Top = 0
    Width = 1016
    Height = 661
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Application'
      object Label6: TLabel
        Left = 336
        Top = 8
        Width = 353
        Height = 19
        Caption = 'Paste your un-transformed AAVSO records here'
      end
      object Label7: TLabel
        Left = 336
        Top = 296
        Width = 480
        Height = 19
        Caption = 
          'The transformed records are here after you hit the Process butto' +
          'n'
      end
      object VersionLabel: TLabel
        Left = 28
        Top = 3
        Width = 83
        Height = 19
        Caption = 'Version 0.1'
      end
      object Memo2: TMemo
        Left = 160
        Top = 328
        Width = 828
        Height = 273
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
        Left = 160
        Top = 40
        Width = 828
        Height = 233
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
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
        Left = 24
        Top = 496
        Width = 105
        Height = 25
        Hint = 'copy the transformed observations to the clipboard'
        Caption = 'To Clipboard'
        TabOrder = 3
        OnClick = ToClipboardButtonClick
      end
      object RadioGroup1: TRadioGroup
        Left = 8
        Top = 24
        Width = 137
        Height = 353
        Caption = 'BVRI'
        TabOrder = 4
      end
      object RadioButton1: TRadioButton
        Left = 24
        Top = 48
        Width = 97
        Height = 17
        Caption = 'Classic'
        Checked = True
        TabOrder = 5
        TabStop = True
      end
      object RadioButton2: TRadioButton
        Left = 24
        Top = 72
        Width = 73
        Height = 17
        Caption = 'New B '
        TabOrder = 6
      end
      object RadioButton3: TRadioButton
        Left = 24
        Top = 96
        Width = 113
        Height = 17
        Caption = 'New B__R'
        TabOrder = 7
      end
      object RadioButton4: TRadioButton
        Left = 24
        Top = 120
        Width = 105
        Height = 17
        Caption = 'New B__I'
        TabOrder = 8
      end
      object RadioButton5: TRadioButton
        Left = 24
        Top = 144
        Width = 73
        Height = 17
        Caption = 'New V'
        TabOrder = 9
      end
      object RadioButton6: TRadioButton
        Left = 24
        Top = 168
        Width = 113
        Height = 17
        Caption = 'New V__I'
        TabOrder = 10
      end
      object RadioButton7: TRadioButton
        Left = 24
        Top = 192
        Width = 97
        Height = 17
        Caption = 'New R'
        TabOrder = 11
      end
      object RadioButton13: TRadioButton
        Left = 24
        Top = 352
        Width = 97
        Height = 17
        Hint = 'Select on Coefficients tab'
        Caption = 'Custom'
        TabOrder = 12
      end
      object IncludeRaw: TCheckBox
        Left = 16
        Top = 392
        Width = 121
        Height = 17
        Hint = 'Include Un transformed obs with Transformed  '
        Caption = 'include raw'
        Enabled = False
        TabOrder = 13
      end
    end
    object Coefficients: TTabSheet
      Caption = 'Coefficients'
      ImageIndex = 2
      object GroupBox1: TGroupBox
        Left = 24
        Top = 16
        Width = 953
        Height = 601
        TabOrder = 0
        object Label19: TLabel
          Left = 608
          Top = 16
          Width = 36
          Height = 19
          Caption = 'error'
        end
        object Label20: TLabel
          Left = 440
          Top = 16
          Width = 79
          Height = 19
          Caption = 'Coefficient'
        end
        object Label15: TLabel
          Left = 792
          Top = 16
          Width = 70
          Height = 19
          Caption = 'Extinction'
        end
        object Label23: TLabel
          Left = 816
          Top = 232
          Width = 38
          Height = 19
          Caption = 'Error'
        end
        object Label24: TLabel
          Left = 768
          Top = 264
          Width = 11
          Height = 19
          Caption = 'B'
        end
        object Label25: TLabel
          Left = 768
          Top = 296
          Width = 11
          Height = 19
          Caption = 'V'
        end
        object Label26: TLabel
          Left = 768
          Top = 328
          Width = 11
          Height = 19
          Caption = 'R'
        end
        object Label27: TLabel
          Left = 773
          Top = 360
          Width = 5
          Height = 19
          Caption = 'I'
        end
        object Label28: TLabel
          Left = 768
          Top = 392
          Width = 11
          Height = 19
          Caption = 'U'
        end
        object Label2: TCheckBox
          Left = 24
          Top = 142
          Width = 390
          Height = 20
          Hint = 'reciprocal of slope of (v-r) versus (V-R)'
          Caption = 'Tvr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 24
        end
        object Label3: TCheckBox
          Left = 24
          Top = 206
          Width = 390
          Height = 20
          Hint = 'reciprocal of slope of (r-i) versus (R-I)'
          Caption = 'Tri'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 25
        end
        object Label4: TCheckBox
          Left = 24
          Top = 350
          Width = 390
          Height = 20
          Hint = 'slope of (V-v) versus (V-R)'
          Caption = 'Tv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 26
        end
        object Label5: TCheckBox
          Left = 24
          Top = 414
          Width = 390
          Height = 20
          Hint = 'slope of (R-r) versus (R-I)'
          Caption = 'Tr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 27
        end
        object Label9: TCheckBox
          Left = 24
          Top = 254
          Width = 390
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 28
        end
        object Label10: TCheckBox
          Left = 24
          Top = 174
          Width = 390
          Height = 20
          Hint = 'reciprocal of slope of (v-i) versus (V-I)'
          Caption = 'Tvi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 29
        end
        object Label8: TCheckBox
          Left = 24
          Top = 382
          Width = 390
          Height = 20
          Hint = 'slope of (V-v) versus (V-I)'
          Caption = 'Tv__I'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 30
        end
        object Label12: TCheckBox
          Left = 24
          Top = 110
          Width = 390
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tbi'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 31
        end
        object Label13: TCheckBox
          Left = 24
          Top = 286
          Width = 390
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb__R'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 32
        end
        object Label14: TCheckBox
          Left = 24
          Top = 318
          Width = 390
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb__I'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 33
        end
        object Label11: TCheckBox
          Left = 24
          Top = 78
          Width = 390
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tbr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 34
        end
        object Label1: TCheckBox
          Left = 24
          Top = 46
          Width = 390
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tbv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 35
        end
        object TbvEdit: TEdit
          Left = 416
          Top = 40
          Width = 121
          Height = 27
          TabOrder = 0
          OnExit = NumericOnExit
        end
        object TvrEdit: TEdit
          Left = 416
          Top = 136
          Width = 121
          Height = 27
          TabOrder = 3
          OnExit = NumericOnExit
        end
        object TriEdit: TEdit
          Left = 416
          Top = 200
          Width = 121
          Height = 27
          TabOrder = 5
          OnExit = NumericOnExit
        end
        object Tv_vrEdit: TEdit
          Left = 416
          Top = 344
          Width = 121
          Height = 27
          TabOrder = 9
          OnExit = NumericOnExit
        end
        object Tr_riEdit: TEdit
          Left = 416
          Top = 408
          Width = 121
          Height = 27
          TabOrder = 11
          OnExit = NumericOnExit
        end
        object Tb_bvEdit: TEdit
          Left = 416
          Top = 248
          Width = 121
          Height = 27
          TabOrder = 6
          OnExit = NumericOnExit
        end
        object TviEdit: TEdit
          Left = 416
          Top = 168
          Width = 121
          Height = 27
          TabOrder = 4
          OnExit = NumericOnExit
        end
        object Tv_viEdit: TEdit
          Left = 416
          Top = 376
          Width = 121
          Height = 27
          TabOrder = 10
          OnExit = NumericOnExit
        end
        object TbrEdit: TEdit
          Left = 416
          Top = 72
          Width = 121
          Height = 27
          TabOrder = 1
          OnExit = NumericOnExit
        end
        object TbiEdit: TEdit
          Left = 416
          Top = 104
          Width = 121
          Height = 27
          TabOrder = 2
          OnExit = NumericOnExit
        end
        object Tb_brEdit: TEdit
          Left = 416
          Top = 280
          Width = 121
          Height = 27
          TabOrder = 7
          OnExit = NumericOnExit
        end
        object Tb_biEdit: TEdit
          Left = 416
          Top = 312
          Width = 121
          Height = 27
          TabOrder = 8
          OnExit = NumericOnExit
        end
        object rTr_riEdit: TEdit
          Left = 576
          Top = 408
          Width = 121
          Height = 27
          TabOrder = 23
          OnExit = NumericOnExit
        end
        object rTv_viEdit: TEdit
          Left = 576
          Top = 376
          Width = 121
          Height = 27
          TabOrder = 22
          OnExit = NumericOnExit
        end
        object rTv_vrEdit: TEdit
          Left = 576
          Top = 344
          Width = 121
          Height = 27
          TabOrder = 21
          OnExit = NumericOnExit
        end
        object rTb_biEdit: TEdit
          Left = 576
          Top = 312
          Width = 121
          Height = 27
          TabOrder = 20
          OnExit = NumericOnExit
        end
        object rTb_brEdit: TEdit
          Left = 576
          Top = 280
          Width = 121
          Height = 27
          TabOrder = 19
          OnExit = NumericOnExit
        end
        object rTb_bvEdit: TEdit
          Left = 576
          Top = 248
          Width = 121
          Height = 27
          TabOrder = 18
          OnExit = NumericOnExit
        end
        object rTriEdit: TEdit
          Left = 576
          Top = 200
          Width = 121
          Height = 27
          TabOrder = 17
          OnExit = NumericOnExit
        end
        object rTviEdit: TEdit
          Left = 576
          Top = 168
          Width = 121
          Height = 27
          TabOrder = 16
          OnExit = NumericOnExit
        end
        object rTvrEdit: TEdit
          Left = 576
          Top = 136
          Width = 121
          Height = 27
          TabOrder = 15
          OnExit = NumericOnExit
        end
        object rTbiEdit: TEdit
          Left = 576
          Top = 104
          Width = 121
          Height = 27
          TabOrder = 14
          OnExit = NumericOnExit
        end
        object rTbrEdit: TEdit
          Left = 576
          Top = 72
          Width = 121
          Height = 27
          TabOrder = 13
          OnExit = NumericOnExit
        end
        object rTbvEdit: TEdit
          Left = 576
          Top = 40
          Width = 121
          Height = 27
          TabOrder = 12
          OnExit = NumericOnExit
        end
        object CheckBox1: TCheckBox
          Left = 24
          Top = 462
          Width = 390
          Height = 20
          Hint = 'slope of (V-v) versus (V-R)'
          Caption = 'Tv'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 36
        end
        object CheckBox2: TCheckBox
          Left = 24
          Top = 494
          Width = 390
          Height = 20
          Hint = 'slope of (V-v) versus (V-I)'
          Caption = 'Tv__I'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 37
        end
        object CheckBox3: TCheckBox
          Left = 24
          Top = 526
          Width = 390
          Height = 20
          Hint = 'slope of (R-r) versus (R-I)'
          Caption = 'Tr'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 38
        end
        object Tv_bvEdit: TEdit
          Left = 416
          Top = 456
          Width = 121
          Height = 27
          TabOrder = 39
          OnExit = NumericOnExit
        end
        object rTv_bvEdit: TEdit
          Left = 576
          Top = 456
          Width = 121
          Height = 27
          TabOrder = 40
          OnExit = NumericOnExit
        end
        object rTr_bvEdit: TEdit
          Left = 576
          Top = 488
          Width = 121
          Height = 27
          TabOrder = 41
          OnExit = NumericOnExit
        end
        object rTi_bvEdit: TEdit
          Left = 576
          Top = 520
          Width = 121
          Height = 27
          TabOrder = 42
          OnExit = NumericOnExit
        end
        object Ti_bvEdit: TEdit
          Left = 416
          Top = 520
          Width = 121
          Height = 27
          TabOrder = 43
          OnExit = NumericOnExit
        end
        object Tr_bvEdit: TEdit
          Left = 416
          Top = 488
          Width = 121
          Height = 27
          TabOrder = 44
          OnExit = NumericOnExit
        end
        object EuEdit: TEdit
          Left = 784
          Top = 168
          Width = 121
          Height = 27
          TabOrder = 45
          OnExit = NumericOnExit
        end
        object EiEdit: TEdit
          Left = 784
          Top = 136
          Width = 121
          Height = 27
          TabOrder = 46
          OnExit = NumericOnExit
        end
        object ErEdit: TEdit
          Left = 784
          Top = 104
          Width = 121
          Height = 27
          TabOrder = 47
          OnExit = NumericOnExit
        end
        object EvEdit: TEdit
          Left = 784
          Top = 72
          Width = 121
          Height = 27
          TabOrder = 48
          OnExit = NumericOnExit
        end
        object EbEdit: TEdit
          Left = 784
          Top = 40
          Width = 121
          Height = 27
          TabOrder = 49
          OnExit = NumericOnExit
        end
        object rEbEdit: TEdit
          Left = 792
          Top = 256
          Width = 121
          Height = 27
          TabOrder = 50
          OnExit = NumericOnExit
        end
        object rEvEdit: TEdit
          Left = 792
          Top = 288
          Width = 121
          Height = 27
          TabOrder = 51
          OnExit = NumericOnExit
        end
        object rErEdit: TEdit
          Left = 792
          Top = 320
          Width = 121
          Height = 27
          TabOrder = 52
          OnExit = NumericOnExit
        end
        object rEiEdit: TEdit
          Left = 792
          Top = 352
          Width = 121
          Height = 27
          TabOrder = 53
          OnExit = NumericOnExit
        end
        object rEuEdit: TEdit
          Left = 792
          Top = 384
          Width = 121
          Height = 27
          TabOrder = 54
          OnExit = NumericOnExit
        end
        object CheckBox4: TCheckBox
          Left = 736
          Top = 46
          Width = 41
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'B'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 55
        end
        object CheckBox5: TCheckBox
          Left = 736
          Top = 78
          Width = 41
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'V'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 56
        end
        object CheckBox6: TCheckBox
          Left = 736
          Top = 110
          Width = 41
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'R'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 57
        end
        object CheckBox7: TCheckBox
          Left = 736
          Top = 142
          Width = 41
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'I'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 58
        end
        object CheckBox8: TCheckBox
          Left = 736
          Top = 174
          Width = 41
          Height = 20
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'U'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -17
          Font.Name = 'Courier New'
          Font.Style = []
          ParentFont = False
          TabOrder = 59
        end
      end
    end
    object Report: TTabSheet
      Caption = 'Report'
      ImageIndex = 2
      object Memo4: TMemo
        Left = 32
        Top = 24
        Width = 961
        Height = 577
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
        Left = 400
        Top = 8
        Width = 75
        Height = 25
        Caption = 'Button2'
        TabOrder = 1
        OnClick = Button2Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Help'
      ImageIndex = 1
      object Memo3: TMemo
        Left = 8
        Top = 32
        Width = 985
        Height = 537
        Lines.Strings = (
          
            'This application is for applying transform coefficients to your ' +
            'AAVSO observations. See the website '
          'http://code.google.com/p/transformapplier/ for more details.'
          ''
          'Inputs:'
          
            '- The input text file is in the format that you would submit to ' +
            'the AAVSO using the WebObs facility. Paste it into the upper mem' +
            'o field '
          'or use the File menu.'
          
            '- Coefficients should be input into the appropriate edit boxes. ' +
            'They will be saved and recovered at start up from an INI file sa' +
            'ved in the '
          
            'same directory as the application. If you don'#39't have a particula' +
            'r coefficient, leave it 0; the program will warn you if it faile' +
            'd because of '
          'lack of coefficients.'
          
            '- Note that the input text needs to contain "#CREFMAG= " stateme' +
            'nts before each observation line so the application will know ho' +
            'w to '
          
            'interpret "ABS" and have the reference magnitude with which to p' +
            'erform the transformation. Actually, you only need to enter the '
          
            'CREFMAG information for a particular comparison star and filter ' +
            'combination once.'
          
            '- Ensemble observations are handled too. You still need CREFMAG,' +
            ' though now it is the reference magnitude of the check star list' +
            'ed '
          'in KNAME.'
          ''
          'Output:'
          
            '- The output appears in the lower memo field. You can click the ' +
            '"Copy to Clipboard" button to have the text available for a past' +
            'e or use '
          'the File/Save menu.'
          ''
          'Equations in the output using the following notation:'
          '-  The target star is referred to as "s"'
          '-  The comparison star is referred to as "c"'
          
            '-  Measured magnitudes have lower case filter letters, upper cas' +
            'e means reference or corrected magnitude. For example Bc is the '
          'reference B filter magnitude of the comparison star.'
          '-  T  is a transform coefficient.'
          '   Tbv= reciprocal of slope of (b-v) versus (B-V)'
          '   Tvr= reciprocal of slope of (v-r) versus (V-R)'
          '   Tvi= reciprocal of slope of (v-i) versus (V-I)'
          '   Tri= reciprocal of slope of (r-i) versus (R-I)'
          '   Tv= slope of (V-v) versus (V-R)'
          '   Tr= slope of (R-r) versus (R-I)'
          '   Tb= slope of (B-b) versus (B-V)'
          '   Tv__I= slope of (V-v) versus (V-I)'
          ''
          ''
          'VERR -'
          
            'While Transformer recomputes VMAG, it does not modify the VERR v' +
            'alue. The use should make any relevant comments in the note '
          'field of the observation record.'
          ''
          ''
          ''
          'How Transformer selects the filter combinations:'
          
            ' - Applying transforms means relating several observations toget' +
            'her. If you are doing a BVRI transform then 4 observations are t' +
            'ied '
          
            'together. Ideally the user should control explicitly this by usi' +
            'ng the Group field.'
          
            ' - If the user does not control this then the program will attem' +
            'pt to group the observations, aiming to do the broadest transfor' +
            'm (ie, as '
          'many filters as possible)'
          ''
          
            ' - Scan the file for all observations of star S/Group G to deter' +
            'mine which filters are available. It is assumed that all the obs' +
            'ervations in '
          'the file can be compared (ie, were done at about the same time)'
          
            ' - Then go through the list of observations, plugging in the dat' +
            'a for that observation in to the group of that filter combinatio' +
            'n.'
          ' So: If you have BVRI obs for S, group 0, of'
          '      B  V  R  I'
          '   1  x'
          '   2     x'
          '   3        x'
          '   4           x'
          '   5  x'
          '   6     x'
          '   The stars used in the transform will be:'
          '   1:  1 6 3 4'
          '   2:  1 2 3 4'
          '   3:  1 2 3 4'
          '   4:  1 2 3 4'
          '   5:  5 2 3 4'
          '   6:  5 6 3 4'
          ''
          'Filter combinations supported:'
          '    BVIR, BV, VR, VRI, BVR, RI, VI, BVI'
          ''
          
            'BVRI   as formulated by Bruce L. Gary (GBL) http://reductionism.' +
            'net.seanic.net/CCD_TE/cte.html'
          
            '  variable notation: filter/star. Star s is the target, c is the' +
            ' comparison. Capital filter is ref, lower case is as observed'
          ' (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]'
          
            ' Rs = rs + (Rc - rc) + Tr * [(Rs - Is) - (Rc - Ic)], using the s' +
            'olution for (Rs - Is) in the above line'
          
            ' Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is' +
            ') from the first line'
          ' Vs = vs + (Vc - vc) + Tv * Tvr * [(vs - rs) - (vc - rc)]'
          ' Bs = Vs + (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]'
          ''
          'BV'
          ' (Bs - Vs) = (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]'
          
            ' Bs = bs + (Bc - bc) + Tb * [(Bs - Vs) - (Bc - Vc)], using the s' +
            'olution for (Bs - Vs) in the above line'
          
            ' Vs = Bs - (Bs - Vs), using Bs from the line above, and (Bs - Vs' +
            ') from the first line'
          ''
          'VR'
          ' (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]'
          
            ' Vs = vs + (Vc - vc) + Tv * [(Vs - Rs) - (Vc - Rc)], using the s' +
            'olution for (Vs - Rs) in the above line'
          
            ' Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs' +
            ') from the first line'
          ''
          'VI    modeled on the RI formulation'
          ' (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]'
          
            ' Vs = vs + (Vc - vc) + Tv__I * [(Vs - Is) - (Vc - Ic)], using th' +
            'e solution for (Vs - Is) in the above line'
          
            ' Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is' +
            ') from the first line'
          ''
          'VRI'
          ' (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]'
          
            ' Rs = rs + (Rc - rc) + Tr * [(Rs - Is) - (Rc - Ic)], using the s' +
            'olution for (Rs - Is) in the above line'
          
            ' Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is' +
            ') from the first line'
          ' Vs = vs + (Vc - vc) + Tv * Tvr * [(vs - rs) - (vc - rc)]'
          ''
          'BVR'
          ' (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]'
          
            ' Vs = vs + (Vc - vc) + Tv * [(Vs - Rs) - (Vc - Rc)], using the s' +
            'olution for (Vs - Rs) in the above line'
          
            ' Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs' +
            ') from the first line'
          ' Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]'
          ''
          'BVI'
          ' (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]'
          
            ' Vs = vs + (Vc - vc) + Tv__I * [(Vs - Is) - (Vc - Ic)], using th' +
            'e solution for (Vs - Is) in the above line'
          
            ' Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is' +
            ') from the first line'
          ' Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]'
          ''
          'RI'
          ' (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]'
          
            ' Rs = rs + (Rc - rc) + Tr * [(Rs - Is) - (Rc - Ic)], using the s' +
            'olution for (Rs - Is) in the above line'
          
            ' Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is' +
            ') from the first line'
          ''
          '')
        ReadOnly = True
        TabOrder = 0
      end
    end
  end
  object Button1: TButton
    Left = 552
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 1
    OnClick = Button1Click
  end
  object MainMenu1: TMainMenu
    Left = 188
    Top = 614
    object File1: TMenuItem
      Caption = '&File'
      object Open1: TMenuItem
        Caption = '&Open un-transformed obs file'
        Hint = 'Open un-transformed observation file'
        OnClick = Open1Click
      end
      object Saveuntransformedobsfile1: TMenuItem
        Caption = 'Save un-transformed obs file'
        OnClick = Saveuntransformedobsfile1Click
      end
      object Save1: TMenuItem
        Caption = '&Save transfomed obs to file'
        Hint = 'Save the transformed observations'
        OnClick = Save1Click
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
    DesignFormWidth = 1024
    DesignFormHeight = 719
    DesignFormClientWidth = 1016
    DesignFormClientHeight = 661
    DesignFormLeft = 309
    DesignFormTop = 149
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = []
    Version = 700
    Left = 12
    Top = 606
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'txt'
    Filter = 'Text files (*.txt)|*.TXT'
    Title = 'Transformed observations'
    Left = 124
    Top = 614
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'txt'
    Filter = 'Text files (*.txt)|*.TXT'
    Title = 'Un-transformed observations'
    Left = 68
    Top = 614
  end
  object HttpCli1: THttpCli
    LocalAddr = '0.0.0.0'
    Agent = 'Mozilla/4.0 (compatible; ICS)'
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
    Left = 36
    Top = 574
  end
end
