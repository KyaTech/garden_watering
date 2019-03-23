struct request {
    unsigned long request_id;
    String attribute_requested;
};

struct response {
    unsigned long request_id;
    String value;
};

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};