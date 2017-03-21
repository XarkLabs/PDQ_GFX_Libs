object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'TTF2GFX 1.0 by Andrey Belokon'
  ClientHeight = 363
  ClientWidth = 674
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  ShowHint = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object SpeedButton1: TSpeedButton
    Left = 264
    Top = 16
    Width = 93
    Height = 22
    Caption = 'Select Font'
    OnClick = SpeedButton1Click
  end
  object Label3: TLabel
    Left = 16
    Top = 127
    Width = 83
    Height = 13
    Caption = 'It'#39's a sample text'
    Color = clCream
    ParentColor = False
    Transparent = False
  end
  object Label2: TLabel
    Left = 16
    Top = 100
    Width = 31
    Height = 13
    Caption = 'Label2'
  end
  object Label4: TLabel
    Left = 16
    Top = 16
    Width = 51
    Height = 13
    Caption = 'Font name'
  end
  object Label5: TLabel
    Left = 16
    Top = 73
    Width = 58
    Height = 13
    Caption = 'Text sample'
  end
  object Button1: TButton
    Left = 387
    Top = 14
    Width = 94
    Height = 25
    Caption = 'Convert!'
    TabOrder = 0
    OnClick = Button1Click
  end
  object edChars: TEdit
    Left = 96
    Top = 43
    Width = 544
    Height = 21
    Enabled = False
    TabOrder = 1
    Text = ' 0123456789.'
  end
  object Edit2: TEdit
    Left = 80
    Top = 16
    Width = 170
    Height = 21
    TabOrder = 2
    Text = 'Edit2'
  end
  object Edit3: TEdit
    Left = 80
    Top = 70
    Width = 378
    Height = 21
    TabOrder = 3
    Text = 'It'#39's a sample text'
    OnChange = Edit3Change
  end
  object cbPacked: TCheckBox
    Left = 496
    Top = 16
    Width = 97
    Height = 17
    Caption = 'Packed'
    TabOrder = 4
  end
  object cbChars: TCheckBox
    Left = 17
    Top = 45
    Width = 82
    Height = 17
    Caption = 'Only chars'
    TabOrder = 5
    OnClick = cbCharsClick
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Options = [fdTrueTypeOnly, fdEffects, fdWysiwyg]
    Left = 272
    Top = 72
  end
end
