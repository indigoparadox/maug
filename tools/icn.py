#!/usr/bin/env python3

import argparse
import logging

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
        # Seek past signature.
        in_bmp_f.seek( 2 )

        # Grab file size.
        in_bmp_sz = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap size: %d bytes', in_bmp_sz )

        # Seek past reserved.
        in_bmp_f.seek( 10 )

        # Grab pixel offset.
        in_bmp_offs = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap pixel offset: %d bytes', in_bmp_offs )

        # Grab header size.
        in_bmp_hdr_sz = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap header size: %d bytes', in_bmp_hdr_sz )

        # Grab image width.
        in_bmp_w = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap width: %d bytes', in_bmp_w )

        # Grab image height.
        in_bmp_h = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap height: %d bytes', in_bmp_h )

        for y in range( 0, 16 ):
            # Seek to the next line (the horizontal lines are backwards, so
            # start at the end of the file and processing horizontal lines
            # in reverse).
            in_bmp_f.seek( in_bmp_offs + (((in_bmp_h - 1) - y) * in_bmp_w) )

            # Process this row.
            for x in range( 0, in_bmp_w ):
                px = int.from_bytes( in_bmp_f.read( 1 ) )
                if 0 < px:
                    print( f"{hex( px )} ", end='' )
                else:
                    print( '    ', end='' )
            print( '' )

if '__main__' == __name__:
    main()

