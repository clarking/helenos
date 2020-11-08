/*
 * Copyright (c) 2020 Jiri Svoboda
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup libui
 * @{
 */
/**
 * @file Image
 */

#include <errno.h>
#include <gfx/bitmap.h>
#include <gfx/context.h>
#include <gfx/text.h>
#include <stdlib.h>
#include <str.h>
#include <ui/control.h>
#include <ui/paint.h>
#include <ui/image.h>
#include "../private/image.h"
#include "../private/resource.h"
#include "../private/ui.h"

static void ui_image_ctl_destroy(void *);
static errno_t ui_image_ctl_paint(void *);
static ui_evclaim_t ui_image_ctl_pos_event(void *, pos_event_t *);

/** Image control ops */
ui_control_ops_t ui_image_ops = {
	.destroy = ui_image_ctl_destroy,
	.paint = ui_image_ctl_paint,
	.pos_event = ui_image_ctl_pos_event
};

/** Create new image.
 *
 * @param resource UI resource
 * @param bitmap Bitmap
 * @param brect Bitmap rectangle
 * @param rimage Place to store pointer to new image
 * @return EOK on success, ENOMEM if out of memory
 */
errno_t ui_image_create(ui_resource_t *resource, gfx_bitmap_t *bitmap,
    gfx_rect_t *brect, ui_image_t **rimage)
{
	ui_image_t *image;
	errno_t rc;

	image = calloc(1, sizeof(ui_image_t));
	if (image == NULL)
		return ENOMEM;

	rc = ui_control_new(&ui_image_ops, (void *) image, &image->control);
	if (rc != EOK) {
		free(image);
		return rc;
	}

	image->bitmap = bitmap;
	image->brect = *brect;
	image->res = resource;
	*rimage = image;
	return EOK;
}

/** Destroy image.
 *
 * @param image Image or @c NULL
 */
void ui_image_destroy(ui_image_t *image)
{
	if (image == NULL)
		return;

	ui_control_delete(image->control);
	if (image->bitmap != NULL)
		gfx_bitmap_destroy(image->bitmap);
	free(image);
}

/** Get base control from image.
 *
 * @param image Image
 * @return Control
 */
ui_control_t *ui_image_ctl(ui_image_t *image)
{
	return image->control;
}

/** Set image rectangle.
 *
 * @param image Image
 * @param rect New image rectangle
 */
void ui_image_set_rect(ui_image_t *image, gfx_rect_t *rect)
{
	image->rect = *rect;
}

/** Paint image.
 *
 * @param image Image
 * @return EOK on success or an error code
 */
errno_t ui_image_paint(ui_image_t *image)
{
	gfx_rect_t srect;
	gfx_coord2_t offs;

	/*
	 * UI image position does not depend on bitmap rectangle p0, so
	 * we need to subtract it.
	 */
	offs.x = image->rect.p0.x - image->brect.p0.x;
	offs.y = image->rect.p0.y - image->brect.p0.y;

	/*
	 * Transalte image rectangle back to bitmap coordinate space.
	 * Thus the bitmap will be clipped to the image rectangle.
	 */
	gfx_rect_rtranslate(&offs, &image->rect, &srect);
	return gfx_bitmap_render(image->bitmap, &srect, &offs);
}

/** Destroy image control.
 *
 * @param arg Argument (ui_image_t *)
 */
void ui_image_ctl_destroy(void *arg)
{
	ui_image_t *image = (ui_image_t *) arg;

	ui_image_destroy(image);
}

/** Paint image control.
 *
 * @param arg Argument (ui_image_t *)
 * @return EOK on success or an error code
 */
errno_t ui_image_ctl_paint(void *arg)
{
	ui_image_t *image = (ui_image_t *) arg;

	return ui_image_paint(image);
}

/** Handle image control position event.
 *
 * @param arg Argument (ui_image_t *)
 * @param pos_event Position event
 * @return @c ui_claimed iff the event is claimed
 */
ui_evclaim_t ui_image_ctl_pos_event(void *arg, pos_event_t *event)
{
	ui_image_t *image = (ui_image_t *) arg;

	(void) image;
	return ui_unclaimed;
}

/** @}
 */
