
import i2ctest
import sys, time

foo = i2ctest.i2ctest()

x0 = 512
y0 = 512
z0 = 512

while 1:
    ret = foo.read_nunchuck()

    x = (ret[2]<<2)|((ret[5]>>2)&0x03)
    y = (ret[3]<<2)|((ret[5]>>4)&0x03)
    z = (ret[4]<<2)|((ret[5]>>6)&0x03)

    x = x - x0
    y = y - y0
    z = z - z0

    absx = -x if x<0 else x
    absy = -y if y<0 else y
    absz = -z if z<0 else z

    maxxy = absx if absx>absy else absy
    maxyz = absy if absy>absz else absz

    minr = maxxy if maxxy>maxyz else maxyz
    maxr = absx + absy + absz

    r2 = absx*absx + absy*absy + absz*absz
    r = (minr + maxr)>>1
    while maxr>minr+1:
        if r*r>r2:
            maxr = r
        else:
            minr = r
        r = (minr + maxr)>>1

    normx = (127*x)/r
    normy = (127*y)/r
    normz = (127*z)/r

    if (r<190) or (r>230):
        dx = 0
        dy = 0
    else:
        if normx>15:
            dx = (normx - 16)>>4
        elif normx<-15:
            dx = (normx + 16)>>4
        else:
            dx = 0
        yy = normy + 40
        if yy>15:
            dy = (yy - 16)>>4
        elif yy<-15:
            dy = (yy + 16)>>4
        else:
            dy = 0

    sys.stdout.write('\x1b[2J\x1b[1;1f')
    print 'x = {0!s}'.format(x)
    print 'y = {0!s}'.format(y)
    print 'z = {0!s}'.format(z)
    print 'r = {0!s}'.format(r)
    print 'normx = {0!s}'.format(normx)
    print 'normy = {0!s}'.format(normy)
    print 'normz = {0!s}'.format(normz)
    print 'C = {0!s}'.format((ret[5]>>1)&0x01)
    print 'Z = {0!s}'.format(ret[5]&0x01)
    print 'dx = {0!s}'.format(dx)
    print 'dy = {0!s}'.format(dy)

    t0 = time.clock()
    while time.clock()<t0+0.05:
        pass

