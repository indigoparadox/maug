#!/usr/bin/env python3

import argparse
import logging

class Bitmap( object ):

    def __init__( self, in_bmp_f ):

        logger = logging.getLogger( 'bitmap.init' )

        # Seek past signature.
        in_bmp_f.seek( 2 )

        # Grab file size.
        self.bmp_sz = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap size: %d bytes', self.bmp_sz )

        # Seek past reserved.
        in_bmp_f.seek( 10 )

        # Grab pixel offset.
        self.px_offset = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap pixel offset: %d bytes', self.px_offset )

        # Grab header size.
        self.header_sz = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap header size: %d bytes', self.header_sz )

        # Grab image width.
        self.width = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap width: %d bytes', self.width )

        # Grab image height.
        self.height = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap height: %d bytes', self.height )

        self._f = in_bmp_f

    def pixels( self, draw_px_cb, pad_ln_cb ):

        for y in range( 0, self.height ):
            # Seek to the next line (the horizontal lines are backwards, so
            # start at the end of the file and processing horizontal lines
            # in reverse).
            self._f.seek(
                self.px_offset + (((self.height - 1) - y) * self.width) )

            # Process this row.
            for x in range( 0, self.width ):
                px = int.from_bytes( self._f.read( 1 ) )
                draw_px_cb( px )
            pad_ln_cb( y )

def main():

    parser = argparse.ArgumentParser()

    parser.add_argument( '-v', '--verbose', action='store_true' )

    parser.add_argument( 'in_bmp' )

    args = parser.parse_args()

    # Setup logging.
    level = logging.WARNING
    if args.verbose:
        level = logging.DEBUG
    logging.basicConfig( level=level )
    logger = logging.getLogger( 'main' )

    # Open the bitmap and parse it.
    with open( args.in_bmp, 'rb' ) as in_bmp_f:

        bmp = Bitmap( in_bmp_f )
        bmp.pixels(
            lambda p: print( f"{hex( p )} ", end='' ) \
                if 0 < p else print( '    ', end='' ),
            lambda y: print( '' ) )

if '__main__' == __name__:
    main()

