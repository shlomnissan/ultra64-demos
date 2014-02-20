#! /usr/sbin/perl
#---------------------------------------------------------------------
#	Copyright (C) 1997, Nintendo.
#	
#	File		ppmtoCI.perl
#	Coded    by	Yoshitaka Yasumoto.	Apr  8, 1997.
#	Modified by	
#	Comments	
#	
#	$Id: ppmtoCI.perl,v 1.3 1997/08/07 03:24:18 yasu Exp $
#---------------------------------------------------------------------
#
#  [Directions for use]
#	perl ppmtoCI.perl width height txtrlabel [tlutoffset] [xlucolor]
#
#       width:      The output width.  
#       height:     The output height.  
#	txtrlabel:  The texture label.  
#	tlutoffset: Use the value after tlutoffset as the Index value.
#		    Default is 1.
#	xlucolor:   The transparent color.  If the transparent color is specified, the Index value of the 
#		    same color pixel as the transparent color is set to 0.  Specify the transparent color 
#		    is *rrggbb.  
#

$argWidth    = $ARGV[0];
$argHeight   = $ARGV[1];
$argLavel    = $ARGV[2];
$argTLUTofs  = $ARGV[3];
$argXLUcolor = $ARGV[4];

# Analyze the ppm file header.  
$i = 0;
while (<STDIN>){
	s/#.*$//;	#The process of the comment.  
	s/^\s+//;	# Relieve the blank. 
	while (s/^\S+//){
		$param[$i ++] = $&;	# Preserve the parameter.  
		s/^\s+//;		# Relieve the blank.  
	}	
	last if ($i == 4);
}
die "This is not P6 ppm format!!\n" if ($param[0] ne "P6");

$width  = $param[1];
$height = $param[2];
$pixel  = $param[1] * $param[2];
$size   = $pixel * 3;

# Acquire the TULT offset value.  
if ($argTLUTofs ne ""){
    $offset = $argTLUTofs;
    $offset = hex($offset) if $offset =~ /^0[xX]/;
} else {
    $offset = 1;
}

# Acquire the transparent color.  
if ($argXLUcolor ne ""){
    if ($argXLUcolor =~ /^#([0-9a-fA-F]{6})/){
	$_ = hex($1);
        $xlucolor = &RGB32to16(($_>>16)&0xff, ($_>>8)&0xff, $_&0xff, 1);
    } else {
	die "Illegal XLU color format. must be \"#rrggbb\".\n";
    }
} else {
    $xlucolor = 0xffffffff;
}

# Output the height and width.  
printf( "/*==========================*\n".
	"    Name  : $argLavel\n".
	"    Width : %d\n".
	"    Height: %d\n".
	" *==========================*/\n", $argWidth, $argHeight);

# Output the texture data.  
printf("unsigned char %s[] = {\n", $argLavel);

# If the size is large, reserve the pallet to make the frame black.  
$c = $offset;
$col = 0;
if ($width < $argWidth || $height < $argHeight){
    $tlut[$c] = 0x0001;
    $pal{0x0001} = $c;
    $c ++;
}

$x0 = ($width  - $argWidth ) / 2;
$y0 = ($height - $argHeight) / 2;

# The conversion to the pallet and output of the Index.  
# Fix the pallet 0 in the transparent color.  
for ($y = $y0; $y < $y0 + $argHeight; $y ++){
    for ($x = $x0; $x < $x0 + $argWidth; $x ++){
	# Judge if it is outside range. 
	if ($y < 0 || $y >= $height || $x < 0 || $x >= $width){
	    # Output black. 
	    $index = $offset;
	} else {
	    # Acquire data for 1-Pixel. 
	    read(STDIN, $buf, 3) == 3 || die "Too short file\n";
	    @tmp = unpack("C3", $buf);
	    
	    # Acquire the color value. 
	    $color = &RGB32to16($tmp[0], $tmp[1], $tmp[2], 1);
	    
	    # Judge the transparent color.  
	    if ($color == $xlucolor){
		$index = 0;
	    } else {
		# Compare the color value with the pallet entry. 
		$index = $pal{$color};
		if ($index == 0){
		    # Register to the pallet. 
		    $index = $pal{$color} = $c;
		    $tlut[$c] = $color;
		    $c ++;
		}
	    }
	}
	# Output
	printf("    ") if $col == 0;    # Indent if it is the header column. 
	printf("0x%02x,", $index);
	if (($col % 4) == 3){
	    printf(" ");
	}
	if ($col ++ == 11){
	    printf("\n");
	    $col = 0;
	}
    }
}
# Align the last column.  
if ($col != 0){
    printf("\n");
}
printf("};\n\n");

# Output the pallet range. 
printf( "/*==========================*\n".
	"    Name      : $argLavel_pal\n".
	"    Index from: 0x%02x (%d)\n".
	"    Index to  : 0x%02x (%d)\n".
	" *==========================*/\n", $offset, $offset, $c-1, $c-1);

# Output the pallet data. 
printf("unsigned short %s_pal[] = {", $argLavel);
$col = 0;
for ($i = $offset; $i < $c; $i ++){
    if (($col % 8) == 0){
	printf("\n    ");
    }
    printf("0x%04x, ", $tlut[$i]);
    $col ++;
}
if ($col != 0){
    printf("\n");
}
printf("};\n\n");


#
#  The conversion routine from RGB 32-bit to RGB 16-bit.  
#
sub RGB32to16 {
   local($r, $g, $b, $a) = @_;
   $r = ($r & 0xf8) << 8;
   $g = ($g & 0xf8) << 3;
   $b = ($b & 0xf8) >> 2;
   $_ = $r | $g | $b | $a;
}

#======== End of ppmtoCI.perl ========
