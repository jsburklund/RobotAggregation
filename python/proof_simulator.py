#!/usr/bin/env python

import numpy as np
import pygame

import argparse


def m2px(m):
    return int(m * 300)


def fwd(x, y, theta, omega, R, dt):
    dtheta = omega * dt
    x_c = x + np.cos(theta + np.pi / 2) * R
    y_c = y + np.sin(theta + np.pi / 2) * R
    x_ = x_c + np.cos(dtheta) * (x - x_c) - np.sin(dtheta) * (y - y_c)
    y_ = y_c + np.sin(dtheta) * (x - x_c) + np.cos(dtheta) * (y - y_c)
    theta_ = theta + dtheta
    theta_ = (theta_ + np.pi) % (2 * np.pi) - np.pi
    return x_, y_, theta_


def fwd_0(x, y, theta, V, l, dt):
    omega = (4 * V) / (3 * l)
    R = -l / 4
    return fwd(x, y, theta, omega, R, dt)


def fwd_1(x, y, theta, V, l, dt):
    omega = (2 * V) / (3 * l)
    R = l
    return fwd(x, y, theta, omega, R, dt)


def sense(x, y, theta, other_x, other_y, r):
    # check whether the ray from x,y,theta intersects the circle of radius r at other_x, other_y
    dy = other_y - y
    dx = other_x - x
    x = np.hypot(dx, dy)
    phi = np.arctan2(dy, dx)
    dphi = np.arctan2(r, x)
    phi_lower = phi - dphi
    phi_upper = phi + dphi

    if phi_lower <= theta <= phi_upper:
        return 1
    else:
        return 0


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("r", type=float, default=0.085)
    parser.add_argument("l", type=float, default=0.014)
    parser.add_argument("dt", type=float, default=0.1)
    parser.add_argument("V", type=float, default=0.02)
    parser.add_argument("j_x", type=float, default=0.5)
    parser.add_argument("j_y", type=float, default=0)
    parser.add_argument("j_theta", type=float, default=0)
    args = parser.parse_args()

    pygame.init()

    BLACK = (0, 0, 0)
    WHITE = (155, 155, 155)
    BLUE = (0, 0, 255)
    GREEN = (0, 255, 0)
    RED = (255, 0, 0)

    W = 600
    size = [W, W]
    screen = pygame.display.set_mode(size)

    done = False
    clock = pygame.time.Clock()

    i_x = 0
    i_y = 0
    i_theta = 0
    i_s = 0
    j_x = args.j_x
    j_y = args.j_y
    j_theta = args.j_theta
    j_s = 0

    ray_length_px = 700

    poses = []
    icc_poses = []
    playing = False
    while not done:
        step = False

        clock.tick(100)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True
            elif event.type == pygame.KEYUP and event.key == pygame.K_p:
                playing = not playing
            elif playing or (event.type == pygame.KEYUP and event.key == pygame.K_RETURN):
                step = True

        i_s = sense(i_x, i_y, i_theta, j_x, j_y, args.r)
        j_s = sense(j_x, j_y, j_theta, i_x, i_y, args.r)

        i_x_px = m2px(args.r) + 20 + m2px(i_x)
        i_y_px = W // 2 + m2px(i_y)
        j_x_px = m2px(args.r) + 20 + m2px(j_x)
        j_y_px = W // 2 + m2px(j_y)
        i_ray_x = i_x_px + np.cos(i_theta) * ray_length_px
        i_ray_y = i_y_px + np.sin(i_theta) * ray_length_px
        j_ray_x = j_x_px + np.cos(j_theta) * ray_length_px
        j_ray_y = j_y_px + np.sin(j_theta) * ray_length_px
        i_icc_x_px = m2px(args.r) + 20 + m2px(i_x + np.cos(i_theta + np.pi / 2) * -args.l / 4)
        i_icc_y_px = W // 2 + m2px(i_y + np.sin(i_theta + np.pi / 2) * -args.l / 4)

        screen.fill(WHITE)
        poses.append([i_x_px, W - i_y_px])
        poses.append([j_x_px, W - j_y_px])
        icc_poses.append([i_icc_x_px, W - i_icc_y_px])
        pygame.draw.circle(screen, BLUE, [i_x_px, W - i_y_px], m2px(args.r))
        if i_s == 0:
            pygame.draw.line(screen, BLACK, [i_x_px, W - i_y_px], [i_ray_x, W - i_ray_y])
        else:
            pygame.draw.line(screen, GREEN, [i_x_px, W - i_y_px], [i_ray_x, W - i_ray_y])
        pygame.draw.circle(screen, RED, [j_x_px, W - j_y_px], m2px(args.r))
        if j_s == 0:
            pygame.draw.line(screen, BLACK, [j_x_px, W - j_y_px], [j_ray_x, W - j_ray_y])
        else:
            pygame.draw.line(screen, GREEN, [j_x_px, W - j_y_px], [j_ray_x, W - j_ray_y])

        for pose in icc_poses:
            pygame.draw.circle(screen, RED, [pose[0], pose[1]], 0)

        for pose in poses:
            pygame.draw.circle(screen, BLACK, [pose[0], pose[1]], 0)

        if step or playing:
            if i_s == 1:
                i_x, i_y, i_theta = fwd_1(i_x, i_y, i_theta, args.V, args.l, args.dt)
            else:
                i_x, i_y, i_theta = fwd_0(i_x, i_y, i_theta, args.V, args.l, args.dt)

            if j_s == 1:
                j_x, j_y, j_theta = fwd_1(j_x, j_y, j_theta, args.V, args.l, args.dt)
            else:
                j_x, j_y, j_theta = fwd_0(j_x, j_y, j_theta, args.V, args.l, args.dt)

        pygame.display.flip()


if __name__ == '__main__':
    main()
