unit Unit1;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.Buttons;

type
  TForm1 = class(TForm)
    Button1: TButton;
    edChars: TEdit;
    FontDialog1: TFontDialog;
    SpeedButton1: TSpeedButton;
    Label3: TLabel;
    Label2: TLabel;
    Label4: TLabel;
    Edit2: TEdit;
    Label5: TLabel;
    Edit3: TEdit;
    cbPacked: TCheckBox;
    cbChars: TCheckBox;
    procedure Button1Click(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Edit3Change(Sender: TObject);
    procedure cbCharsClick(Sender: TObject);
  private
    procedure ShowFont;
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

type
  TLogPal = record
    lpal : TLogPalette;
    colorSpace : Array[0..255] of TPaletteEntry; // This allocate room to
                                                 // new palette colors
                                                 // since palPalEntry member of
                                                 // TLogPalette is declared as
                                                 // Array [0..0] of TPaletteEntry
  end;

  TGFXglyph = record
    packed_data: Boolean;
	  bitmapOffset: integer;     // Pointer into GFXfont->bitmap
	  width, height: integer;    // Bitmap dimensions in pixels
	  xAdvance: integer;         // Distance to advance cursor (x axis)
	  xOffset, yOffset: integer; // Dist from cursor pos to UL corner
  end;

function IsEmptyRow(bmp: TBitmap; y,x1,x2: integer): Boolean;
var j: integer;
begin
  for j:=x1 to x2 do
    if PBYTE(bmp.ScanLine[y])[j] = 0 then Exit(False);
  Result:=True;
end;

function IsEmptyCol(bmp: TBitmap; y1,y2,x: integer): Boolean;
var j: integer;
begin
  for j:=y1 to y2 do
    if PBYTE(bmp.ScanLine[j])[x] = 0 then Exit(False);
  Result:=True;
end;

function FoundGlyphDim(bmp: TBitmap; var r: TRect): Boolean;
var y,x: integer;
begin
  while (r.Top <= r.Bottom) and IsEmptyRow(bmp,r.Top,r.Left,r.Right) do
    Inc(r.Top);
  if r.Top > r.Bottom then Exit(False);
  while (r.Bottom >= r.Top) and IsEmptyRow(bmp,r.Bottom,r.Left,r.Right) do
    Dec(r.Bottom);
  while (r.Left <= r.Right) and IsEmptyCol(bmp,r.Top,r.Bottom,r.Left) do
    Inc(r.Left);
  while (r.Right >= r.Left) and IsEmptyCol(bmp,r.Top,r.Bottom,r.Right) do
    Dec(r.Right);
  Result:=True;
end;

procedure SaveGlyph(bmp: TBitmap; r: TRect; stream: TStream);
var
  mask,buf: Byte;
  y,x: integer;
begin
  mask:=$80; buf:=0;
  for y:=r.Top to r.Bottom do begin
    for x:=r.Left to r.Right do begin
      if PBYTE(bmp.ScanLine[y])[x] = 0 then
        buf:=buf or mask;
      mask:=mask shr 1;
      if mask = 0 then begin
        stream.Write(buf,1);
        mask:=$80; buf:=0;
      end;
    end;
  end;
  if mask <> $80 then
    stream.Write(buf,1);
end;

procedure SaveGlyphPacked(bmp: TBitmap; r: TRect; stream: TStream);
var
  buf,curb: Byte;
  y,x,cnt,bufcnt: integer;

  procedure Flush;
  begin
    if curb = 0 then
      buf:=(buf shl 4) or ((cnt-1) or $8)
    else
      buf:=(buf shl 4) or (cnt-1);
    Inc(bufcnt);
    if bufcnt = 2 then begin
      // swap 4-bits
      buf:=((buf shr 4) and $F) or ((buf and $F) shl 4);
      stream.Write(buf,1);
      bufcnt:=0;
      buf:=0;
    end;
  end;

begin
  bufcnt:=0; buf:=0;
  curb:=PBYTE(bmp.ScanLine[r.Top])[r.Left];
  cnt:=0;
  for y:=r.Top to r.Bottom do begin
    for x:=r.Left to r.Right do begin
      if (cnt >= 8) or (PBYTE(bmp.ScanLine[y])[x] <> curb) then begin
        // flush
        Flush;
        curb:=PBYTE(bmp.ScanLine[y])[x];
        cnt:=0;
      end;
      Inc(cnt);
    end;
  end;
  Flush;
  if bufcnt > 0 then
    stream.Write(buf,1);
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  bmp: TBitmap;
  ch: char;
  chars,str,fn: string;
  j,min_chr,max_chr,baseline,width_A,w,h,sz,maxh: integer;
  gl: TArray<TGFXglyph>;
  gd: TRect;
  sb: TStringBuilder;
  pal : TLogPal;
  stream: TMemoryStream;
  p: PBYTE;
  stra: AnsiString;
  spos: int64;
begin
  // Create a 256 gray-scale palette
  pal.lpal.palVersion:=$300;
  pal.lpal.palNumEntries := 256;
  for j := 0 to 255 do begin
    with pal.lpal.palPalEntry[j] do
      begin
        peRed := j;
        peGreen := j;
        peBlue := j;
      end;
  end;
  if cbChars.Checked then
    chars:=edChars.Text
  else begin
    chars:='';
    for j:=$20 to $7F do
      chars:=chars+AnsiChar(j);
  end;
  min_chr:=MAXINT;
  max_chr:=-1;
  for j:=1 to Length(chars) do begin
    if Ord(chars[j]) < min_chr then
      min_chr:=Ord(chars[j]);
    if Ord(chars[j]) > max_chr then
      max_chr:=Ord(chars[j]);
  end;
  SetLength(gl,max_chr-min_chr+1);
  sb:=TStringBuilder.Create;
  stream:=TMemoryStream.Create;
  bmp:=TBitmap.Create;
  try
    bmp.SetSize(100,100);
    bmp.PixelFormat:=pf8bit;
    bmp.Palette:=CreatePalette(pal.lpal);
    bmp.Canvas.Brush.Color:=clWhite;
    bmp.Canvas.Brush.Style:=bsSolid;
    bmp.Canvas.Font:=FontDialog1.Font;
    // found baseline
    baseline:=bmp.Canvas.TextHeight('A');
    width_A:=bmp.Canvas.TextWidth('A');
    maxh:=0;
    // draw chars
    for j:=0 to High(gl) do begin
      ch:=Chr(min_chr+j);
      if Pos(ch,chars) > 0 then begin
        bmp.Canvas.FillRect(Rect(0,0,100,100));
        str:='A'+ch;
        bmp.Canvas.TextOut(0,0,str);
        w:=bmp.Canvas.TextWidth(str);
        h:=bmp.Canvas.TextHeight(str);
        if h > maxh then maxh:=h;
        gd:=Rect(width_A,0,w-1,h-1);
        gl[j].bitmapOffset:=stream.Position;
        if FoundGlyphDim(bmp,gd) then begin
          gl[j].width:=gd.Width+1;
          gl[j].height:=gd.Height+1;
          gl[j].xOffset:=gd.Left-width_A;
          gl[j].yOffset:=gd.Top-baseline;
          gl[j].packed_data:=False;
          if cbPacked.Checked then begin
            spos:=stream.Position;
            SaveGlyphPacked(bmp,gd,stream);
            if stream.Size-spos > ((gl[j].height*gl[j].width+7) div 8) then begin
              stream.Position:=spos;
              stream.Size:=spos;
              SaveGlyph(bmp,gd,stream);
            end else
              gl[j].packed_data:=True;
          end else
            SaveGlyph(bmp,gd,stream);
        end else begin
          gl[j].width:=0;
          gl[j].height:=0;
          gl[j].xOffset:=0;
          gl[j].yOffset:=0;
        end;
        gl[j].xAdvance:=w-width_A;
      end else begin
        gl[j]:=gl[0];
      end;
    end;
    // генерируем текст
    sb.Append('// Font charset: ');
    sb.Append(chars);
    sb.AppendLine;
    sb.AppendLine;
    sb.Append('// font size ').Append(stream.Size).Append(' bytes').AppendLine;
    sb.Append('const uint8_t '+Edit2.Text+'Bitmaps[] PROGMEM = {');
    sb.AppendLine;
    sz:=stream.Size;
    p:=stream.Memory;
    for j:=0 to sz-1 do begin
      sb.Append('0x'+IntToHex(p[j],2));
      if j <> sz-1 then sb.Append(', ');
      if (j and $F) = $F then sb.AppendLine.Append('  ');
    end;
    sb.Append('};');
    sb.AppendLine;
    //
    sb.AppendLine;
    sb.Append('const GFXglyph '+Edit2.Text+'Glyphs[] PROGMEM = {');
    sb.AppendLine;
    for j:=0 to High(gl) do begin
      sb.Append('{');
      if gl[j].packed_data then
        sb.Append(gl[j].bitmapOffset or $8000)
      else
        sb.Append(gl[j].bitmapOffset);
      sb.Append(', ');
      sb.Append(gl[j].width);
      sb.Append(', ');
      sb.Append(gl[j].height);
      sb.Append(', ');
      sb.Append(gl[j].xAdvance);
      sb.Append(', ');
      sb.Append(gl[j].xOffset);
      sb.Append(', ');
      sb.Append(gl[j].yOffset);
      sb.Append('}');
      if j <> High(gl) then sb.Append(',');
      sb.Append('  // ');
      sb.Append(Char(min_chr+j));
      sb.Append(', ');
      if j < High(gl) then
        sb.Append(gl[j+1].bitmapOffset-gl[j].bitmapOffset)
      else
        sb.Append(stream.Size-gl[j].bitmapOffset+1);
      sb.AppendLine;
    end;
    sb.Append('};');
    sb.AppendLine;
    //
    sb.AppendLine;
    sb.Append('const GFXfont '+Edit2.Text+' PROGMEM = {');
    sb.AppendLine;
    sb.Append('  (uint8_t  *)'+Edit2.Text+'Bitmaps,');
    sb.AppendLine;
    sb.Append('  (GFXglyph  *)'+Edit2.Text+'Glyphs,');
    sb.AppendLine;
    sb.Append('  ').Append(min_chr).Append(', ').Append(max_chr).Append(', ').Append(maxh);
    sb.AppendLine;
    sb.Append('};');
    sb.AppendLine;
    stra:=sb.ToString;
  finally
    bmp.Free;
    sb.Free;
    stream.Free;
  end;
  if cbPacked.Checked then
    fn:=Edit2.Text+'.pck.h'
  else
    fn:=Edit2.Text+'.h';
  with TFileStream.Create(fn,fmCreate) do try
    Write(stra[1],Length(stra));
  finally
    Free;
  end;
end;

procedure TForm1.cbCharsClick(Sender: TObject);
begin
  edChars.Enabled:=cbChars.Checked;
end;

procedure TForm1.Edit3Change(Sender: TObject);
begin
  Label3.Caption:=Edit3.Text;
  Label2.Caption:='Height '+IntToStr(Label3.Height)+', width '+IntToStr(Label3.Width);
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  ShowFont;
end;

procedure TForm1.ShowFont;
var
  str: string;
  j: integer;
begin
  Label3.Font:=FontDialog1.Font;
  str:=FontDialog1.Font.Name+IntToStr(FontDialog1.Font.Size);
  for j:=1 to Length(str) do
    if str[j] = ' ' then str[j]:='_';
  Edit2.Text:=str;
  Edit3Change(nil);
end;

procedure TForm1.SpeedButton1Click(Sender: TObject);
begin
  if FontDialog1.Execute then ShowFont;
end;

end.
