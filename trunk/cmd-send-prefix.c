/* $Id: cmd-send-prefix.c,v 1.10 2008-06-03 05:35:51 nicm Exp $ */

/*
 * Copyright (c) 2007 Nicholas Marriott <nicm@users.sourceforge.net>
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

#include "tmux.h"

/*
 * Send prefix key as a key.
 */

void	cmd_send_prefix_exec(void *, struct cmd_ctx *);

const struct cmd_entry cmd_send_prefix_entry = {
	"send-prefix", NULL,
	CMD_CLIENTONLY_USAGE,
	0,
	cmd_clientonly_parse,
	cmd_send_prefix_exec,
	cmd_clientonly_send,
	cmd_clientonly_recv,
	cmd_clientonly_free,
	NULL
};

void
cmd_send_prefix_exec(void *ptr, struct cmd_ctx *ctx)
{
	struct client	*c;

	if ((c = cmd_clientonly_get(ptr, ctx)) == NULL)
		return;

	window_key(c->session->curw->window, prefix_key);

	if (ctx->cmdclient != NULL)
		server_write_client(ctx->cmdclient, MSG_EXIT, NULL, 0);
}
