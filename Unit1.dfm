object Form1: TForm1
  Left = 10
  Top = 38
  Width = 1036
  Height = 721
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
  PixelsPerInch = 96
  TextHeight = 19
  object CoefPage: TPageControl
    Left = 0
    Top = 0
    Width = 1028
    Height = 667
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Application'
      object Label6: TLabel
        Left = 336
        Top = 8
        Width = 355
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
        Width = 84
        Height = 19
        Caption = 'Version 0.1'
      end
      object Memo2: TMemo
        Left = 288
        Top = 328
        Width = 700
        Height = 273
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        WordWrap = False
      end
      object Memo1: TMemo
        Left = 288
        Top = 40
        Width = 700
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
        Left = 96
        Top = 384
        Width = 145
        Height = 25
        Hint = 'Transform  the untransformed'
        Caption = 'Process'
        TabOrder = 2
        OnClick = ProcessButtonClick
      end
      object ToClipboardButton: TButton
        Left = 96
        Top = 488
        Width = 147
        Height = 25
        Hint = 'copy the transformed observations to the clipboard'
        Caption = 'To Clipboard'
        TabOrder = 3
        OnClick = ToClipboardButtonClick
      end
      object GroupBox1: TGroupBox
        Left = 32
        Top = 40
        Width = 225
        Height = 321
        Caption = 'Coefficients'
        TabOrder = 4
        object Label1: TLabel
          Left = 24
          Top = 30
          Width = 25
          Height = 19
          Hint = 'reciprocal of slope of (b-v) versus (B-V)'
          Caption = 'Tbv'
        end
        object Label2: TLabel
          Left = 24
          Top = 62
          Width = 22
          Height = 19
          Hint = 'reciprocal of slope of (v-r) versus (V-R)'
          Caption = 'Tvr'
        end
        object Label3: TLabel
          Left = 24
          Top = 126
          Width = 19
          Height = 19
          Hint = 'reciprocal of slope of (r-i) versus (R-I)'
          Caption = 'Tri'
        end
        object Label4: TLabel
          Left = 24
          Top = 158
          Width = 16
          Height = 19
          Hint = 'slope of (V-v) versus (V-R)'
          Caption = 'Tv'
        end
        object Label5: TLabel
          Left = 24
          Top = 190
          Width = 15
          Height = 19
          Hint = 'slope of (R-r) versus (R-I)'
          Caption = 'Tr'
        end
        object label9: TLabel
          Left = 24
          Top = 222
          Width = 18
          Height = 19
          Hint = 'slope of (B-b) versus (B-V)'
          Caption = 'Tb'
        end
        object Label10: TLabel
          Left = 24
          Top = 94
          Width = 20
          Height = 19
          Hint = 'reciprocal of slope of (v-i) versus (V-I)'
          Caption = 'Tvi'
        end
        object Label8: TLabel
          Left = 24
          Top = 254
          Width = 39
          Height = 19
          Hint = 'slope of (V-v) versus (V-I)'
          Caption = 'Tv__I'
        end
        object TbvEdit: TEdit
          Left = 80
          Top = 24
          Width = 121
          Height = 27
          TabOrder = 0
          OnExit = NumericOnExit
        end
        object TvrEdit: TEdit
          Left = 80
          Top = 56
          Width = 121
          Height = 27
          TabOrder = 1
          OnExit = NumericOnExit
        end
        object TriEdit: TEdit
          Left = 80
          Top = 120
          Width = 121
          Height = 27
          TabOrder = 3
          OnExit = NumericOnExit
        end
        object TvEdit: TEdit
          Left = 80
          Top = 152
          Width = 121
          Height = 27
          TabOrder = 4
          OnExit = NumericOnExit
        end
        object TrEdit: TEdit
          Left = 80
          Top = 184
          Width = 121
          Height = 27
          TabOrder = 5
          OnExit = NumericOnExit
        end
        object TbEdit: TEdit
          Left = 80
          Top = 216
          Width = 121
          Height = 27
          TabOrder = 6
          OnExit = NumericOnExit
        end
        object TviEdit: TEdit
          Left = 80
          Top = 88
          Width = 121
          Height = 27
          TabOrder = 2
          OnExit = NumericOnExit
        end
        object Tv__IEdit: TEdit
          Left = 80
          Top = 248
          Width = 121
          Height = 27
          TabOrder = 7
          OnExit = NumericOnExit
        end
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
            'AAVSO observations. See the website'
          'http://www.gasilvis.net/transformer/ for more'
          'details.'
          ''
          'Inputs:'
          
            '- The input text file is in the format that you would submit to ' +
            'the AAVSO using the WebObs facility. Paste it into the upper mem' +
            'o field'
          'or use the'
          'File/Save menu.'
          
            '- Coefficients should be input into the appropriate edit boxes. ' +
            'They will be saved and recovered at start up from an INI file sa' +
            'ved in the'
          'same directory'
          
            'as the application. If you don'#39't have a particular coefficient, ' +
            'leave it 0; the program will warn you if it failed because of la' +
            'ck of'
          'coefficients.'
          
            '- Note that the input text needs to contain "#CREFMAG= " stateme' +
            'nts before each observation line so the application will know ho' +
            'w to'
          'interpret "ABS"'
          
            'and have the reference magnitude with which to perform the trans' +
            'formation. Actually, you only need to enter the CREFMAG'
          'information for a'
          'particular comparison star and filter combination once.'
          
            '- Ensemble observations are handled too. You still need CREFMAG,' +
            ' though now it is the reference magnitude of the check star list' +
            'ed'
          'in KNAME.'
          ''
          'Output:'
          
            '- The output appears in the lower memo field. You can click the ' +
            '"Copy to Clipboard" button to have the text available for a past' +
            'e or use '
          'the File/Save '
          'menu.'
          ''
          'Equations in the output using the following notation:'
          '-  The target star is referred to as "s"'
          '-  The comparison star is referred to as "c"'
          
            '-  Measured magnitudes have lower case filter letters, upper cas' +
            'e means reference or corrected magnitude. For example Bc is the '
          'reference B filter '
          'magnitude of the comparison star.'
          '-  T_  is a transform coefficient.'
          '   Tbv= reciprocal of slope of (b-v) versus (B-V)'
          '   Tvr= reciprocal of slope of (v-r) versus (V-R)'
          '   Tvi= reciprocal of slope of (v-i) versus (V-I)'
          '   Tri= reciprocal of slope of (r-i) versus (R-I)'
          '   Tv= slope of (V-v) versus (V-R)'
          '   Tr= slope of (R-r) versus (R-I)'
          '   Tb= slope of (B-b) versus (B-V)'
          ''
          ''
          ''
          'VERR -'
          
            'While Transformer recomputes VMAG, it does not modify the VERR v' +
            'alue. The use should make any relevant comments in the note '
          'field of the '
          'observation record.'
          ''
          ''
          ''
          'How Transformer selects the filter combinations:'
          
            ' - Applying transforms means relating several observations toget' +
            'her. If you are doing a BVRI transform then 4 observations are t' +
            'ied '
          'together. Ideally '
          
            'the user should control explicitly this by using the Group field' +
            '.'
          
            ' - If the user does not control this then the program will attem' +
            'pt to group the observations, aiming to do the broadest transfor' +
            'm (ie, as '
          'many filters as '
          'possible)'
          ''
          
            ' - Scan the file for all observations of star S/Group G to deter' +
            'mine which filters are available. It is assumed that all the obs' +
            'ervations in '
          'the file can be '
          'compared (ie, were done at about the same time)'
          
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
          
            ' Vs = vs + (Vc - vc) + Tv * [(Vs - Is) - (Vc - Ic)], using the s' +
            'olution for (Vs - Is) in the above line'
          
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
          
            ' Vs = vs + (Vc - vc) + Tv * [(Vs - Is) - (Vc - Ic)], using the s' +
            'olution for (Vs - Is) in the above line'
          
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
          ''
          ''
          ''
          ''
          ' ')
        ReadOnly = True
        TabOrder = 0
      end
    end
  end
  object MainMenu1: TMainMenu
    Left = 52
    Top = 486
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
    DesignFormWidth = 1036
    DesignFormHeight = 721
    DesignFormClientWidth = 1028
    DesignFormClientHeight = 667
    DesignFormLeft = 10
    DesignFormTop = 38
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
    Left = 76
    Top = 526
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'txt'
    Filter = 'Text files (*.txt)|*.TXT'
    Title = 'Un-transformed observations'
    Left = 140
    Top = 542
  end
end
