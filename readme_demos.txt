
The function and characteristic of each sample program is explained below:

autofill:	This is a simple sample program that measures the performance of painting out
 		the screen.  It paints out the screen using a polygon with different aspect
 		ratio with size, and measures the speed automatically.  It is possible to see
 		the measurement data using gnuplot.  For details,  refer to README in the
 		autofill sample program.

blockmonkey:	This is an application that measures a simple performance.  A user can change
 		the size of a display list or parameters, and confirm the effect when
		rendering.  Operation instructions for the controller are displayed on the host
 		screen when the application is executed.
		The graph of performance measurement on the lower screen  shows the time needed
 		to render a polygon.   The processing time of CPU needed  to create a display
 		list is not calculated.  This is not to slow down the animation even when it
 		cannot catch up with the next frame.

bumpmap:	This is a sample program of bump mapping.  For details, refer to the README in
 		the bumpmap sample program.

chrome:		This is a sample program for the reflection mapping function.

ci8fb:		This sample program is similar to autofill.  Rendering is processed using 8bit
 		color index.  For details, refer to the README for the ci8fb sample program. 

detail:		This is a sample program which explains how to use the multi tiling texture.
   		This demonstrates the detail texture and MIP map texture as well.  

eeptest:	This is a simple program which tests EEPROM (4k or 16k).  

fault:		This is a sample program which processes the CPU exception without using  rmon
 		debug thread.  
 
fogworld:	This is a sample program which shows how to use the fog process and the volume
 		culling process. 

ginv:		This is a simple sample program which outputs such information as the version
 		of the Emulator board equipped on Indy, clock speed of RCP and CPU, and the
 		total amount of available DRAM.

ground:		This is a sample program which performs the clipping of a large ground plane
 		accurately.

gs2dex:		This is a sample program which explains how to use the sprite micro code S2DEX.

gtdemo:		This is a complicated sample program in which characters will run or punch
 		someone.  This demonstrates the multiple graphics task, and turbo3D micro code
 		is used a great deal.  Animation is realized using a complex matrix calculation
 		by the host.

hostio:		This sample program exchanges data between Indy (host) and the game application
 		using the Host-U64 API (Application programmer's interface).

lights:		This expresses specular highlight effect which uses RSP texture, and shadows
 		(shades) falsely.  By using inputs from the controller, it controls the
 		location and size.  The background is processed with MIP map.  

mipmap:		This demonstrates MIP mapping.  It is possible to verify by comparing with
 		other texture processing or changing MIP map parameters.   

morphcube:	CPU calculates each vertex for morphing.  CPU and the graphics pipeline execute
 		2 frames consecutively in parallel.  Tiling of a large MIP map texture is
 		processed  with a capacity that is stored in a texture memory.

morphfaces:	Morphing of two dimensional images. CPU processes the morphing of vertex, and
 		blending is performed using 2 cycle mode. For details, refer to NOTES in the
 		morphfaces sample program.  

motortest:	A sample program for Rumble Pak.

nnsample1:	A sample program for the NTSC scheduler.   

nnsample2:	A sample program for graphics micro code F3DEX Ver1.21. 

nosPak:		A sample program using N64 font data.

onetri:		A simple sample program for rendering 1 polygon without Z buffer.  By executing
 		using "gload -a -t", a simple texture mapping is possible.  
        
pfs:		A sample program to demonstrate the Controller Pak interface.

playseq:	A sample program which uses a sequence player.

polyline:	This renders by switching micro codes of polygon and line.

simple:		This demonstrates the compatibility of the audio process and the graphics
 		process.  In this sample, the audio process is supposed to stop the graphics
 		process when the graphics process cannot be done in time.

soundmonkey:	A sample program which uses both a sequence player and a sound player.

spgame:		A sample program which renders objects in the foreground and the background
  		using a sprite function.  

spritemonkey:	A sample program which demonstrates use of sprite micro code Sprite2D.  
 
sramtest:	This tests operation of 256K SRAM.  The result is returned to the terminal by
 		osSyncPrintf. The 256K SRAM board is necessary to execute it.

terrain:	This is a sample program for the creation of geographical features.  This
 		renders the geometry using N64 hardware, and operates the interface using the
 		Controller.  It is also possible to create data and store it in the Indy disk
 		using this tool.  It demonstrates many advanced functions such as 
             	communication with the host (HOST I/O).   For details, refer to the README in
 		the terrain sample program.  

texlight:	A sample program which expresses a diffuse lighting process, a specular
 		highlight process,  and objects which are texture mapped at the same time.   
             
tile_rect2d:	This demonstrates in a simple way how to divide a large texture into small
 		texture tiles so it can be stored in a texture memory.    It creates two
 		dimensional texture rectangle primitives.

tile_rect3d:	This explains how to divide a large texture into tiles so it can be stored in a
 		texture memory.  As filtering is processed properly, there will be no joints.
  		A triangle will be created.    

topgun: 	The fighter in this sample program is the .flt format data for which modeling
 		was processed by MultiGen.   A display list is created using a geometry
 		converter flt2c which is attached as a sample.   
 
tron:		A wall is a transparent object which is not sort processed.    It demonstrates 		how to use the BOWTIE_VAL parameter, which avoids RDP hanging.  

turbomonkey:	This is the same as blockmonkey except that is uses turbo3D micro code.  This 		is a sample program that verifies the difference between turbo3D micro code and 		Fast3D micro code on the actual screen.  A trade-off of image quality or 		processing speed can be compared using this program and blockmonkey. 
        
 

