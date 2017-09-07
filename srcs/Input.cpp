/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Input.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cledant <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/03 13:18:00 by cledant           #+#    #+#             */
/*   Updated: 2017/09/07 13:10:19 by cledant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Input.hpp"

Input::Input(void) : p_key(1024, RELEASED), p_mouse(32, RELEASED), pos_x(0.0f), pos_y(0.0f), last_pos_x(0.0f), last_pos_y(0.0f), timer(0.0f)
{
}

Input::~Input(void)
{
}
