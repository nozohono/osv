#include "drivers/virtio.hh"
#include "debug.hh"

using namespace pci;

bool
Virtio::earlyInitChecks() {
    if (!Driver::earlyInitChecks()) return false;

    u8 rev;
    if (getRevision() != VIRTIO_PCI_ABI_VERSION) {
        debug(fmt("Wrong virtio revision=%x") % rev);
        return false;
    }

    if (_id < VIRTIO_PCI_ID_MIN || _id > VIRTIO_PCI_ID_MAX) {
        debug(fmt("Wrong virtio dev id %x") % _id);

        return false;
    }

    debug(fmt("%s passed. Subsystem: vid:%x:id:%x") % __FUNCTION__ % (u16)getSubsysVid() % (u16)getSubsysId());
    return true;
}

bool
Virtio::Init(Device* dev) {

    if (!earlyInitChecks()) return false;

    if (!Driver::Init(dev)) return false;

    debug(fmt("Virtio:Init %x:%x") % _vid % _id);

    _bars[0]->write(VIRTIO_PCI_STATUS, (u8)(VIRTIO_CONFIG_S_ACKNOWLEDGE |
            VIRTIO_CONFIG_S_DRIVER));

    _bars[0]->write(VIRTIO_PCI_STATUS, (u8)(VIRTIO_CONFIG_S_DRIVER_OK));

    return true;
}

void Virtio::dumpConfig() const {
    Driver::dumpConfig();
    debug(fmt("Virtio vid:id= %x:%x") % _vid % _id);
}

void
Virtio::vring_init(struct vring *vr, unsigned int num, void *p, unsigned long align) {
    vr->num = num;
    vr->desc = reinterpret_cast<struct vring_desc*>(p);
    vr->avail = reinterpret_cast<struct vring_avail*>(p) + num*sizeof(struct vring_desc);
    vr->used = reinterpret_cast<struct vring_used*>(((unsigned long)&vr->avail->ring[num] + sizeof(u16) \
        + align-1) & ~(align - 1));
}

unsigned
Virtio::vring_size(unsigned int num, unsigned long align) {
    return ((sizeof(struct vring_desc) * num + sizeof(u16) * (3 + num)
         + align - 1) & ~(align - 1))
        + sizeof(u16) * 3 + sizeof(struct vring_used_elem) * num;
}

/* The following is used with USED_EVENT_IDX and AVAIL_EVENT_IDX */
/* Assuming a given event_idx value from the other size, if
 * we have just incremented index from old to new_idx,
 * should we trigger an event? */
int
Virtio::vring_need_event(u16 event_idx, u16 new_idx, u16 old) {
    /* Note: Xen has similar logic for notification hold-off
     * in include/xen/interface/io/ring.h with req_event and req_prod
     * corresponding to event_idx + 1 and new_idx respectively.
     * Note also that req_event and req_prod in Xen start at 1,
     * event indexes in virtio start at 0. */
    return (u16)(new_idx - event_idx - 1) < (u16)(new_idx - old);
}

