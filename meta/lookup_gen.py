output = "const u16 idx_lookup[ 224 ][ 320 ] = {\n"

for y in range( 0, 224 ):
    output += "\t{ "
    for x in range( 0, 320 ):
        cellY = y // 8
        cellX = x // 8

        posY = y % 8
        posX = x % 8

        output += str(
            ( ( 1280 * cellY ) + ( 32 * cellX ) ) + ( posY * 4 ) + ( posX // 2 )
        )

        if x != 319:
            output += ", "

    output += " }"
    if y != 223:
        output += ",\n"

output += " };"

outfile = open( "result.c", "w" )
outfile.write( output )
outfile.close()
