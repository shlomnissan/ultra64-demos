#! /usr/sbin/perl
#---------------------------------------------------------------------
#	Copyright (C) 1997, Nintendo.
#	
#	File		ppm8to5.perl
#	Coded    by	Yoshitaka Yasumoto.	Apr 10, 1997.
#	Modified by	
#	Comments	
#	
#	$Id: ppm8to5.perl,v 1.2 1997/08/07 03:24:17 yasu Exp $
#---------------------------------------------------------------------
#
#  [Directions for use]
#	perl ppm8to5.perl < infile > outfile
#
#	Round the 8-bit RGB to the 5-bit RGB.  
#

# Analyze the ppm file header.  
$i = 0;
while (<STDIN>){
	s/#.*$//;	# The process of the comment.  
	s/^\s+//;	# Relieve the bland.  
	while (s/^\S+//){
		$param[$i ++] = $&;	# Preserve the parameter.  
		s/^\s+//;		# Relive the bland.  
	}	
	last if ($i == 4);
}
die "This is not P6 ppm format!!\n" if ($param[0] ne "P6");

$width  = $param[1];
$height = $param[2];
$pixel  = $param[1] * $param[2];

printf("P6\n%d %d\n255\n", $width, $height);

for ($i = 0; $i < $pixel; $i ++){

    # Acquire data for 1-pixel.  
    read(STDIN, $buf, 3) == 3 || die "Too short file\n";
    @tmp = unpack("C3", $buf);
    
    # Acquire the color value.  
    $tmp[0] &= 0xf8;
    $tmp[1] &= 0xf8;
    $tmp[2] &= 0xf8;

    # The conversion to binary.
    $buf = pack("C3", @tmp);    
	    
    # Output.  
    print $buf;
}

#======== End of ppm8to5.perl ========
