OPTION _EXPLICIT

TYPE Vector2i
    x AS INTEGER
    y AS INTEGER
END TYPE

TYPE Vector2f
    x AS DOUBLE
    y AS DOUBLE
END TYPE

DIM x AS INTEGER
DIM y AS INTEGER

DIM scr AS Vector2i
DIM position AS Vector2f
DIM direction AS Vector2f
DIM plane AS Vector2f

scr.x = 1152
scr.y = 864

SCREEN _NEWIMAGE(scr.x,scr.y,256)

DIM map(15, 15) AS INTEGER

FOR y = 0 TO 15
    FOR x = 0 TO 15
        READ map(x, y)
    NEXT x
NEXT y

position.x = 10
position.y = 5
direction.x = -1
direction.y = 0
plane.x = 0
plane.y = 0.66

WHILE 1
    _LIMIT 30
    FOR x = 0 TO scr.x
        DIM camera AS Vector2f
        DIM rayPos AS Vector2f
        DIM rayDir AS Vector2f

        ' Calculate ray position and direction
        camera.x = 2 * x / scr.x - 1
        rayPos.x = position.x
        rayPos.y = position.y
        rayDir.x = direction.x + plane.x * camera.x
        rayDir.y = direction.y + plane.y * camera.x

        ' DDA Algorithm
        DIM map AS Vector2i
        map.x = rayPos.x
        map.y = rayPos.y

        DIM sideDist AS Vector2f
        DIM deltaDist AS Vector2f

        deltaDist.x = SQR( 1 + ( rayDir.y * rayDir.y ) / ( rayDir.x * rayDir.x ) )
        deltaDist.y = SQR( 1 + ( rayDir.x * rayDir.x ) / ( rayDir.y * rayDir.y ) )

        DIM perpWallDist AS DOUBLE
        DIM stp AS Vector2i
        DIM hit AS INTEGER
        DIM side AS INTEGER
    NEXT x
WEND

DATA 7,2,1,2,1,1,1,2,1,1,1,1,1,1,1,1
DATA 7,0,0,0,2,2,2,2,2,6,0,0,0,0,0,3
DATA 7,0,0,0,0,5,0,0,3,6,0,0,0,0,0,3
DATA 7,0,0,0,0,0,0,0,3,6,0,3,0,3,0,3
DATA 7,0,0,0,0,5,0,0,3,6,0,0,0,0,0,3
DATA 7,2,2,2,2,3,0,0,3,6,0,0,0,0,0,3
DATA 1,4,1,1,1,1,7,0,7,6,0,0,0,0,7,3
DATA 3,4,7,0,0,0,7,0,7,6,0,0,0,0,0,3
DATA 3,0,3,5,0,5,7,0,7,7,0,0,0,0,0,3
DATA 3,0,0,5,0,0,0,0,0,0,0,0,0,0,0,3
DATA 3,0,0,5,5,5,5,1,5,7,0,0,0,0,0,3
DATA 5,0,5,5,2,2,2,2,2,6,0,0,0,0,0,3
DATA 5,0,0,0,7,0,0,0,0,0,0,3,0,3,0,3
DATA 5,0,0,0,0,0,0,0,0,6,0,0,0,0,0,3
DATA 5,0,0,0,7,0,0,0,0,6,0,0,0,0,0,3
DATA 5,5,5,5,2,2,2,2,2,1,1,1,1,1,1,6

