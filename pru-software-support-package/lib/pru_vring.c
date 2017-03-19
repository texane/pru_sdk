/*
 * pru_vring.c - VRING support functions
 */

#include "pru_vring.h"

void pru_vring_init(struct pru_vring *pvr, const char *name,
	const struct fw_rsc_vdev_vring *rsc_vring)
{
	vring_init(&pvr->vr, rsc_vring->num,
		(void *)rsc_vring->da, rsc_vring->align);
	pvr->align = rsc_vring->align;
	pvr->notifyid = rsc_vring->notifyid;
	pvr->num_mask = rsc_vring->num - 1;
	pvr->last_avail = 0;
	pvr->last_avail_orig = 0;
	pvr->avail_idx_p = &pvr->vr.avail->idx;
	pvr->used_idx_p = &pvr->vr.used->idx;
	pvr->avail_ring_p = pvr->vr.avail->ring;
}

void pru_vring_elem_init(struct pru_vring *pvr,
		struct pru_vring_elem *pvre)
{
	u16 num = pvr->vr.num;

	pvre->idx = num;

	pvre->out_num = 0;
	pvre->out_idx = num;
	pvre->out_len = 0;

	pvre->in_num = 0;
	pvre->in_idx = num;
	pvre->in_len = 0;

	/* save in case of rollback */
	pvr->last_avail_orig = pvr->last_avail;
}

int pru_vring_pop(struct pru_vring *pvr, struct pru_vring_elem *pvre)
{
	u16 i, head;
	struct vring_desc *vrd;

	if (!pru_vring_num_heads(pvr, pvr->last_avail))
		return 0;

	pru_vring_elem_init(pvr, pvre);

	/* get head */
	head = pru_vring_get_head(pvr, pvr->last_avail++);

	vrd = pru_vring_desc(pvr, head);

	i = head;

	pvre->idx = i;

	/* loop collecting elements */
	for (;;) {
		if (vrd->flags & VRING_DESC_F_WRITE) {
			if (pvre->in_num++ == 0)
				pvre->in_idx = i & pvr->num_mask;
			pvre->in_len += vrd->len;
		} else {
			if (pvre->out_num++ == 0)
				pvre->out_idx = i & pvr->num_mask;
			pvre->out_len += vrd->len;
		}

		if (!(vrd->flags & VRING_DESC_F_NEXT))
			break;

		/* next descriptor */
		vrd = pru_vring_desc(pvr, ++i);
	}

	return pvre->in_num + pvre->out_num;
}

void pru_vring_push(struct pru_vring *pvr,
		const struct pru_vring_elem *pvre, u32 len)
{
	u16 idx, old;
	struct vring_used_elem *used_vring;

	/* first fill */
	old = pru_vring_used_idx(pvr);
	idx = old & pvr->num_mask;

	used_vring = &pvr->vr.used->ring[idx];
	used_vring->id = pvre->idx & pvr->num_mask;
	used_vring->len = len;

	/* now update */
	pru_vring_used_idx_set(pvr, ++old);
}

#ifdef DEBUG
void dump_vring(const char *name, struct vring *vring, unsigned int align)
{
	struct vring_desc *vd;
	int i, sz;

	sz = vring_size(vring->num, align);
	sc_printf("vring %s @0x%x num %d size %d desc=0x%x avail=0x%x used=0x%x",
		name, (unsigned int)vring->desc, vring->num, sz,
		(unsigned int)vring->desc, (unsigned int)vring->avail,
		(unsigned int)vring->used);

	for (i = 0; i < vring->num; i++) {
		vd = &vring->desc[i];
		sc_printf(" d#%d: a 0x%x l 0x%x f 0x%x n 0x%x",
			i, (__u32)vd->addr, vd->len, vd->flags, vd->next);
	}
	sc_printf(" avail: flags 0x%x idx 0x%x",
		vring->avail->flags, vring->avail->idx);

	for (i = 0; i < vring->num; i++) {
		sc_printf(" a#%d: ring 0x%x", i,
			vring->avail->ring[i]);
	}
	sc_printf(" avail: used_event_idx 0x%x",
		vring->avail->ring[vring->num]);

	sc_printf(" used: flags 0x%x idx 0x%x",
		vring->used->flags, vring->used->idx);
	for (i = 0; i < vring->num; i++) {
		sc_printf(" u#%d: id 0x%x len 0x%x", i,
			vring->used->ring[i].id,
				vring->used->ring[i].len);
	}

}
#endif
