/* $Id: cmd-generic.c,v 1.4 2008-06-03 16:55:09 nicm Exp $ */

/*
 * Copyright (c) 2008 Nicholas Marriott <nicm@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include <getopt.h>
#include <stdlib.h>

#include "tmux.h"

struct cmd_clientonly_data {
	char	*cname;
};

struct cmd_sessiononly_data {
	char	*cname;
	char	*sname;
};

struct cmd_windowonly_data {
	char	*cname;
	char	*sname;
	int	 idx;
};

int
cmd_clientonly_parse(
    struct cmd *self, void **ptr, int argc, char **argv, char **cause)
{
	struct cmd_clientonly_data	*data;
	int				 opt;

	*ptr = data = xmalloc(sizeof *data);
	data->cname = NULL;

	while ((opt = getopt(argc, argv, "c:")) != EOF) {
		switch (opt) {
		case 'c':
			if (data->cname == NULL)
				data->cname = xstrdup(optarg);
			break;
		default:
			goto usage;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 0)
		goto usage;

	return (0);

usage:
	xasprintf(cause, "usage: %s %s", self->entry->name, self->entry->usage);

	self->entry->free(data);
	return (-1);
}

void
cmd_clientonly_send(void *ptr, struct buffer *b)
{
	struct cmd_clientonly_data	*data = ptr;

	buffer_write(b, data, sizeof *data);
	cmd_send_string(b, data->cname);
}

void
cmd_clientonly_recv(void **ptr, struct buffer *b)
{
	struct cmd_clientonly_data	*data;

	*ptr = data = xmalloc(sizeof *data);
	buffer_read(b, data, sizeof *data);
	data->cname = cmd_recv_string(b);
}

void
cmd_clientonly_free(void *ptr)
{
	struct cmd_clientonly_data	*data = ptr;

	if (data->cname != NULL)
		xfree(data->cname);
	xfree(data);
}

struct client *
cmd_clientonly_get(void *ptr, struct cmd_ctx *ctx)
{
	struct cmd_clientonly_data	*data = ptr;

  	if (data != NULL)
		return (cmd_find_client(ctx, data->cname));
	return (cmd_find_client(ctx, NULL));
}

int
cmd_sessiononly_parse(
    struct cmd *self, void **ptr, int argc, char **argv, char **cause)
{
	struct cmd_sessiononly_data	*data;
	int				 opt;

	*ptr = data = xmalloc(sizeof *data);
	data->cname = NULL;
	data->sname = NULL;

	while ((opt = getopt(argc, argv, "c:s:")) != EOF) {
		switch (opt) {
		case 'c':
			if (data->sname != NULL)
				goto usage;
			if (data->cname == NULL)
				data->cname = xstrdup(optarg);
			break;
		case 's':
			if (data->cname != NULL)
				goto usage;
			if (data->sname == NULL)
 				data->sname = xstrdup(optarg);
			break;
		default:
			goto usage;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 0)
		goto usage;

	return (0);

usage:
	xasprintf(cause, "usage: %s %s", self->entry->name, self->entry->usage);

	self->entry->free(data);
	return (-1);
}

void
cmd_sessiononly_send(void *ptr, struct buffer *b)
{
	struct cmd_sessiononly_data	*data = ptr;

	buffer_write(b, data, sizeof *data);
	cmd_send_string(b, data->cname);
	cmd_send_string(b, data->sname);
}

void
cmd_sessiononly_recv(void **ptr, struct buffer *b)
{
	struct cmd_sessiononly_data	*data;

	*ptr = data = xmalloc(sizeof *data);
	buffer_read(b, data, sizeof *data);
	data->cname = cmd_recv_string(b);
	data->sname = cmd_recv_string(b);
}

void
cmd_sessiononly_free(void *ptr)
{
	struct cmd_sessiononly_data	*data = ptr;

	if (data->cname != NULL)
		xfree(data->cname);
	if (data->sname != NULL)
		xfree(data->sname);
	xfree(data);
}

struct session *
cmd_sessiononly_get(void *ptr, struct cmd_ctx *ctx)
{
	struct cmd_sessiononly_data	*data = ptr;

  	if (data != NULL)
		return (cmd_find_session(ctx, data->cname, data->sname));
	return (cmd_find_session(ctx, NULL, NULL));
}

int
cmd_windowonly_parse(
    struct cmd *self, void **ptr, int argc, char **argv, char **cause)
{
	struct cmd_windowonly_data	*data;
	int				 opt;
	const char			*errstr;

	*ptr = data = xmalloc(sizeof *data);
	data->cname = NULL;
	data->sname = NULL;
	data->idx = -1;

	while ((opt = getopt(argc, argv, "c:i:s:")) != EOF) {
		switch (opt) {
		case 'c':
			if (data->sname != NULL)
				goto usage;
			if (data->cname == NULL)
				data->cname = xstrdup(optarg);
			break;
		case 'i':
			data->idx = strtonum(optarg, 0, INT_MAX, &errstr);
			if (errstr != NULL) {
				xasprintf(cause, "index %s", errstr);
				goto error;
			}
			break;
		case 's':
			if (data->cname != NULL)
				goto usage;
			if (data->sname == NULL)
				data->sname = xstrdup(optarg);
			break;
		default:
			goto usage;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 0)
		goto usage;

	return (0);

usage:
	xasprintf(cause, "usage: %s %s", self->entry->name, self->entry->usage);

error:
	self->entry->free(data);
	return (-1);
}

void
cmd_windowonly_send(void *ptr, struct buffer *b)
{
	struct cmd_windowonly_data	*data = ptr;

	buffer_write(b, data, sizeof *data);
	cmd_send_string(b, data->cname);
	cmd_send_string(b, data->sname);
}

void
cmd_windowonly_recv(void **ptr, struct buffer *b)
{
	struct cmd_windowonly_data	*data;

	*ptr = data = xmalloc(sizeof *data);
	buffer_read(b, data, sizeof *data);
	data->cname = cmd_recv_string(b);
	data->sname = cmd_recv_string(b);
}

void
cmd_windowonly_free(void *ptr)
{
	struct cmd_windowonly_data	*data = ptr;

	if (data->cname != NULL)
		xfree(data->cname);
	if (data->sname != NULL)
		xfree(data->sname);
	xfree(data);
}

struct winlink *
cmd_windowonly_get(void *ptr, struct cmd_ctx *ctx, struct session **sp)
{
	struct cmd_windowonly_data	*data = ptr;
	struct winlink			*wl;

  	if (data == NULL) {
		wl = cmd_find_window(ctx, NULL, NULL, -1, sp);
		return (wl);
	}
		
	return (cmd_find_window(ctx, data->cname, data->sname, data->idx, sp));
}