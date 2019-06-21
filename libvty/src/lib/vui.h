#ifndef _VUI_H_
#define _VUI_H_

typedef struct vui_s {
} vui_t;

vui_t *vui_new();
void vui_delete(vui_t *vui);
int vui_serv_stdio(vui_t *vui);
int vui_serv_sock(vui_t *vui, uint32_t addr, uint16_t port);
int vui_set_password(vui_t *vui, const char* password);
int vui_set_hostname(vui_t *vui, const char* hostname);
void vui_install_node(vui_t *vui, struct cmd_node *node);
void vui_install_element(vui_t *vui,
    enum node_type ntype, struct cmd_element *cmd);

#endif /* _VUI_H_ */
