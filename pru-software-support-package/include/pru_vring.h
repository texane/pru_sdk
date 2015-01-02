
#ifndef PRU_VRING_H
#define PRU_VRING_H

#include "pru_virtio_ring.h"
#include "rsc_types.h"
#include "types.h"

struct pru_vring {
	struct vring vr;
	const char *name;
	uint32_t align;
	uint32_t notifyid;

	uint16_t num_mask;
	uint16_t last_avail;
	uint16_t last_avail_orig;

	uint16_t last_used_idx;

	uint16_t * volatile avail_idx_p;
	uint16_t * volatile used_idx_p;
	uint16_t * volatile avail_ring_p;
};

struct pru_vring_elem {

	uint32_t out_len;	/* sum length of out descs */
	uint16_t out_num;	/* number of out descs */
	uint16_t out_idx;	/* index of first out desc */

	uint32_t in_len;	/* sum length on in descs */
	uint16_t in_num;	/* number of in descs */
	uint16_t in_idx;	/* index of first in desc */

	uint16_t idx;	/* first idx */
};

static inline uint16_t pru_vring_avail_idx(struct pru_vring *pvr)
{
	uint16_t val;

	val = *pvr->avail_idx_p;
	return val;
}

static inline uint16_t pru_vring_used_idx(struct pru_vring *pvr)
{
	return *pvr->used_idx_p;
}

static inline void pru_vring_used_idx_set(struct pru_vring *pvr, uint16_t idx)
{
	*pvr->used_idx_p = idx;
}

static inline uint16_t pru_vring_avail_ring(struct pru_vring *pvr, uint16_t i)
{
	return pvr->avail_ring_p[i];
}

static inline struct vring_desc *pru_vring_desc(struct pru_vring *pvr, uint16_t idx)
{
	return &pvr->vr.desc[idx & pvr->num_mask];
}

/* if > num something is very very wrong */
static inline uint16_t pru_vring_num_heads(struct pru_vring *pvr, uint16_t idx)
{
	uint16_t num_heads = pru_vring_avail_idx(pvr) - idx;

	return num_heads;
}

static inline uint16_t pru_vring_get_head(struct pru_vring *pvr, uint16_t idx)
{
	uint16_t head = pru_vring_avail_ring(pvr, idx & pvr->num_mask);

	return head;
}

static inline uint8_t
pru_vring_buf_is_avail(struct pru_vring *pvr)
{
	return pru_vring_num_heads(pvr, pvr->last_avail) != 0;
}

static inline struct vring_desc *
pru_vring_get_next_avail_desc(struct pru_vring *pvr)
{
	return pru_vring_desc(pvr, pru_vring_get_head(pvr, pvr->last_avail++));
}

static inline void pru_vring_push_one(struct pru_vring *pvr, uint32_t len)
{
	uint16_t old, idx;
	struct vring_used_elem *vru;

	/* first fill */
	old = pru_vring_used_idx(pvr);
	idx = old & pvr->num_mask;

	vru = &pvr->vr.used->ring[idx];
	vru->id = idx & pvr->num_mask;
	vru->len = len;

	/* now update */
	pru_vring_used_idx_set(pvr, ++old);
}

void pru_vring_init(struct pru_vring *pvr, const char *name,
		const struct fw_rsc_vdev_vring *rsc_vring);
void pru_vring_elem_init(struct pru_vring *pvr, struct pru_vring_elem *pvre);
int pru_vring_pop(struct pru_vring *pvr, struct pru_vring_elem *pvre);
void pru_vring_push(struct pru_vring *pvr, const struct pru_vring_elem *pvre,
		uint32_t len);

#endif
