# [Project_S16_K23a](https://github.com/dimitriosliapis/Project1_S16_K23a)

* __Γαβαλάς Αλέξης__ @AlexGavalas
    ΑΜ 1115201000004
* __Λαυτσής Πάρις__ @in7ictus
    ΑΜ 1115201000167
* __Λιάπης Δημήτρης__ @dimitriosliapis
    ΑΜ 1115201100038

### Δομές

* __Buffer__
    Η δομή αποθήκευσης της λίστας γειτνίασης του κάθε κόμβου στο γράφο. Είναι πίνακας από `ListNodes` του οποίου το αρχικό μέγεθος δηλώνεται στο αρχείο `buffer.h` ως `BUFF_SIZE`, και διπλασιάζεται δυναμικά όταν και όσες φορές κριθεί απαραίτητο.

    * `list_node_t` _(Το struct των κόμβων του buffer)_
    Ο τύπος των κόμβων της λίστας γειτνίασης. Αποτελείται από έναν πίνακα `neighbor[N]`, όπου αποθηκεύονται οι γείτονες, έναν πίνακα `edgeProperty[N]`, όπου αποθηκεύεται η έκδοση κάθε ακμής, και ένα offset `nextListNode` του επόμενου ListNode, όπου εάν έχει γεμίσει η λίστα του αρχικού, τότε το χρησιμοποιεί για να αποθηκεύσει τους υπόλοιπους γείτονες, κ.ο.κ.

    * `createBuffer()`  _(Δημιουργία και αρχικοποίηση buffer)_
    Η αρχική τιμή που δίνεται στα ID (`neighbor[i]`) των γειτόνων είναι η `DEFAULT` η οποία είναι ίση με `2^24`. Η επιλογή αυτή έγινε με βάση την παραδοχή από την εκφώνηση πως η μεγαλύτερη τιμή που μπορεί να πάρει ένα ID είναι η `2^24 - 1`. Ο δείκτης `nextListNode` αρχικοποιείται σε -1. Επιστρέφεται ο δείκτης στο Buffer.

    * `allocNewNode()`  _(Εύρεση offset επόμενης κενής θέσης στο buffer)_
    Η λειτουργία της συγκεκριμένης συνάρτησης είναι να ελέγχει εάν χωράει στο Buffer ο επόμενος κόμβος. Αν χωράει επιστρέφει το offset, αλλιώς καλεί την συνάρτηση που αυξάνει το μέγεθός του.

    * `reallocBuffer()` _(Διπλασιασμός μεγέθους buffer)_
    Δέχεται το μέγεθος του Buffer το οποίο διπλασιάζει, αλλάζει την τιμή του μεγέθους του και τέλος αρχικοποιεί τις καινούριες θέσης μνήμης.

    * `destroyBuffer()` _(Απελευθέρωση μνήμης buffer)_

* __Index__
    Η δομή του ευρετηρίου που χρησιμοποιείται για την ανάκτηση των γειτόνων από τη δομή του buffer. Είναι πίνακας από `struct index_t` ή αλλιώς `ind`. Έχει ένα αρχικό μέγεθος `IND_SIZE`, το οποίο δηλώνεται στο αρχείο `index.h` και διπλασιάζεται δυναμικά όταν και όσες φορές κριθεί απαραίτητο.

    * `index_t` _(Το struct των κόμβων του index)_
    Κρατάει για κάθε έναν το offset του πρώτου `first` και του τελευταίου `last` κόμβου της λίστας γειτνίασής του στο Buffer, έναν πίνακα `visited[THREAD_POOL_SIZE + 1]` για να μαρκάρεται ο κόμβος από τα worker threads, ένα hash table `neighbors` για αποδοτικότερο έλεγχο ύπαρξης κάποιου γείτονα, και έναν μετρητή `num_of_children` που κρατάει τον συνολικό αριθμό των γειτόνων του. Τέλος, κρατάει έναν δείκτη `s_data` σε δομή `static_data_t`.

    * `static_data_t`   _(Το struct για την έξτρα πληροφορία σε στατικό γράφο)_
    Κρατάει τα `lowlink`, `index`, `curr_neighbors`, `n`, και `onStack` για τον αλγόριθμο _Tarjan_, και τα `rank`, `min_rank`, `all_children_in_scc[NUM_GRAIL]` και `children_in_scc` για την δημιουργία του _Ευρετηρίου Grail_.

    * `createNodeIndex()`   _(Δημιουργία και αρχικοποίηση index)_
    Η δημιουργία και οι αρχικοποιήσεις εξαρτόνται από το αν ο γράφος είναι στατικός ή δυναμικός

    * `lookup()`    _(Ύπαρξη ή μη κόμβου στο index)_
    Επιστρέφει αν υπάρχει ή όχι ο κόμβος με `id` στο Ευρετήριο. Αν το `id` είναι μεγαλύτερο από το μέγεθος του index τότε προφανώς ο κόμβος αυτός δεν υπάρχει.

    * `insertNode()`    _(Εισαγωγή κόμβου στο index)_
    Βρίσκει τον πρώτο ελεύθερο κόμβο στο Buffer μέσω της `allocNewNode()` και μετά εισάγει στη θέση του πίνακα index με αριθμό `id` την τιμή του offset του επόμενου ελέυθερου κόμβου στο buffer. Αν χρειαστεί, αν δηλαδή το `id` είναι μεγαλύτερο από το μέγεθος του index, αυξάνει το μέγεθος του πίνακα `index`. Αυξάνει επίσης την βοηθητική μεταβλητή `available` η οποία δίνει την δυνατότητα εύρεσης του offset του επόμενου κόμβου που θα εισαχθεί στο Buffer σε χρόνο Ο(1).

    * `reallocNodeIndex()`  _(Αύξηση μεγέθους index)_
    Διπλασιάζει το μέγεθος του πίνακα τόσες φορές όσες χρειάζεται για να χωρέσει το `id`. Αφού γίνει αυτό αρχικοποιεί τις καινούριες θέσεις του πίνακα και αποθηκεύει το νέο μέγεθος.

    * `addEdge()`   _(Προσθήκη ακμής γείτονα)_
    Αρχικά βρίσκει το offset του πρώτου κόμβου της λίστας γειτνίασης στο buffer για το δοθέν `id`. Αν το όριθμα `check` είναι ίσο με 1, τότε θα ελέγξει με τη βοήθεια του hash table `neighbors` αν η ακμή υπάρχει ήδη. Αλλιώς το `id` του γείτονα (`neighbor`) εισάγεται στην πρώτη διαθέσιμη θέση στον πίνακα γειτόνων του buffer.

    * `getListHead()`   _(Εύρεση offset πρώτου κόμβου της λίστας γειτνίασης)_

    * `destroyNodeIndex()`  _(Απελευθέρωση μνήμης index)_

* __Hash Table__
    Η δομή που χρησιμοποιείται για τον αποδοτικότερο έλεγχο ύπαρξης γείτονα. Το αρχικό του μέγεθος δηλώνεται στο αρχείο `hash.h` ως `HT_SMALL`, ενώ το μέγεθος κάθε bucket ως `HT_N`, και διπλασιάζεται δυναμικά όταν και όσες φορές κριθεί απαραίτητο.

    * `createHashtable()`   _(Δημιουργία hash table)_

    * `search()`    _(Ύπαρξη ή μη κόμβου στο hash table)_

    * `insert()`    _(Εισαγωγή κόμβου στο hash table)_

    * `delete()`    _(Απελευθέρωση μνήμης του hash table)_
    
* __Stack__
    Η δομή που χρησιμοποιείται για την υλοποίηση στοίβας, που βοηθάει στη δημιουργία των SCC, καθώς και στην αποφυγή αναδρομής α) κατά την δημιουργία των CC και β) κατά την αναζήτηση στο UpdateIndex. Το αρχικό της μέγεθος δηλώνεται στο αρχείο `cc.h` ως `STACK_ARRAY_SIZE`, και διπλασιάζεται δυναμικά όταν και όσες φορές κριθεί απαραίτητο.

    * `Stack_t` _(Το struct της στοίβας)_

    * `createStack()`   _(Δημιουργία στοίβας)_

    * `stackisempty()`  _(Έλεχγος για το αν η στοίβα είναι άδεια)_

    * `pushinstack()`   _(Εισαγωγή στοιχείου στην στοίβα)_

    * `popfromstack()`  _(Εξαγωγή στοιχείου από την στοίβα)_
    
    * `peekfromstack()` _(Επιστροφή του τελυταίου στοιχείου που εισήχθει στη στοίβα χωρίς εξαγωγή)_
    
    * `deletestack()`   _(Απελευθέρωση μνήμης της στοίβας)_
    
* __Queue__
    Η δομή που χρησιμοποιείται για την υλοποίηση κυκλικής ουράς, που αναπαριστά το σύνορο στον BFS. Το αρχικό της μέγεθος δηλώνεται στο αρχείο `search.h` ως `QUEUE_SIZE`, και διπλασιάζεται δυναμικά όταν και όσες φορές κριθεί απαραίτητο.
    
    * `queue_t` _(Το struct της ουράς)_
    
    * `createQueue()`   _(Δημιουργία ουράς FIFO με δείκτες στο πρώτο και στο τελευταίο στοιχείο)_
    
    * `isEmpty()`   _(Έλεγχος για το αν η ουρά είναι άδεια)_
    
    * `enq()`   _(Εισαγωγή στοιχείου στο τέλος της ουράς)_
    
    * `deq()`   _(Εξαγωγή του πρώτου στοιχείου της ουράς)_
    
    * `empty()` _(Απελευθέρωση μνήμης για όλα τα στοιχεία της ουράς καθώς και για την ίδια της ουρά)_
    
* __Strongly Connected Components__

    * `Component_t` _(H δομή Component αποθηκεύει το id του component και τα ids των κόμβων που εμπεριέχονται σε αυτό)_
        
    * `SCC_t`   _(Η δομή SCC αποθηκεύει την πληροφορία των components και χρησιμοποιείται για την απάντηση των σχετικών ερωτημάτων. Πιο συγκεκριμένα, αποθηκεύει για κάθε component ποιους κόμβους του γραφήματος αυτό εμπεριέχει)_
    
    * `estimateStronglyConnectedComponents_iterative()` _(Η συνάρτηση που καλεί τον αλγόριθμο Tarjan)_
    
    * `destroyStronglyConnectedComponents()`    _(Απελευθέρωση μνήμης του SCC)_
    
* __Grail Index__
    
    * `GrailIndex_t`    _(Το struct του υπεργάφου, όπου κάθε κόμβος απεικονίζει ένα SCC του αρχικού γράφου)_
        
    * `shuffle()`   _(Ανακάτεμα στοιχείων πίνακα)_
        
    * `buildGrailIndex()` _(Δημιουργία 'NUM_GRAIL' ευρετηρίων grail)_
    
    * `destroyGrailIndex()` _(Απελευθέρωση μνήμης grail index)_
    
* __CC Index__

    * `CC_ind_t`    _(Περιέχει το id του κάθε CC)_
    
    * `u_unode_t`   _(Δομή που κρατάει για κάθε CC με ποιά άλλα συνδέεται μετά από προσθήκη ακμών (`cc_array`) και ένα πίνακα με τους καινούριους κόμβους που θα προστεθούν σε αυτό το CC (`new_nodes`))_
    
    * `CC_t`    _(Δομή που περιέχει το CC index και το Update Index καθώς και τα μεγέθη τους και την τιμή του METRIC)_

    * `createCCIndex()` _(Δημιουργία του CCIndex με τους κόμβους του γράφου)_

    * `initUpdateIndex()`   _(Δημιουργία και αρχικοποίηση του UpdateIndex)_

    * `refreshUpdateIndex()`    _(Προσθήκη αλλαγών στο `updateIndex`. Είτε σύνδεση 2 CC που ήδη υπάρχουν, είτε προσθήκη νέων. Αντίστοιχα για τους νέους κόμβους)_

    * `searchUpdateIndex()` _(Αναζήτηση στο Update Index για την πιθανή σύνδεση μεταξύ 2 κόμβων)_
    
    * `updateCCIndex()` _(Αναδιάταξη και προσθήκη όσων δεν ήταν στο CC Index μέσω του Update Index όταν το χρησιμοποιηθεί το Update Index `ΜΕΤRIC` φορές)_
    
    * `destroyCCIndex()`    _(Απελευθέρωση μνήμης του CC Index.)_

### Αλγόριθμοι

* __Bidirectional Breadth-First-Search__

    * `bBFS()`
    Διεξάγει δύο αναζητήσεις BFS ταυτόχρονα. Μία απο το κόμβο προορισμού και μία απο τον κόμβο προέλευσης. Σε κάθε βήμα επιλέγει την πλευρά ανάπτυξης με βάση τον αριθμό των κόμβων που αναμένεται να επεκτείνει. Σε κάθε μία, ο κόμβος εκκίνησης τοποθετείται στο σύνορο, μαρκάρεται (ανάλογα με το `version` στην θέση `thread_id` του πίνακα `visited`) και στη συνέχεια, μέχρι να αδειάσει κάποιο από τα δύο σύνορα επαναλαμβάνεται η εξής διαδικασία: με την βοήθεια των `counterF`, `counterFS`, `counterB` και `counterBS` εξάγονται από το εκάστοτε σύνορο οι κόμβοι που μπήκαν στο προηγούμενο βάθος. Για κάθε έναν από τους γείτονές τους, ελέγχεται αν έχουν ξαναεπισκεφτεί και αν όχι, τότε μαρκάρονται, ενώ κατόπιν ελέγχονται αν έχουν επισκεφτεί από τον άλλο BFS. Αν συμβαίνει κάτι τέτοιο τότε έχει βρεθεί μονοπάτι, αλλιώς ο γείτονας τοποθετείται στο σύνορο, κ.ο.κ. Σε περίπτωση που αδειάσει κάποιο από τα δύο σύνορα, τότε δεν υπάρχει μονοπάτι.
    _Σημείωση: Σε στατικό γράφο, ανάλογα με με το αποτέλεσμα της `isReachableGrailIndex()`, στην περίπτωση ΥΕΣ η αναζήτηση περιορίζεται σε ένα SCC, ενώ στην περίπτωση ΜΑΥΒΕ, για κάθε επίπεδο που επεκτείνεται ξανακαλείται η `isReachableGrailIndex()` και πράττει αναλόγως._

* __Tarjan__
    
    * `tarjan_iterative()`
    Διεξάγει αναζήτηση DFS επαναληπτικά, αναθέτοντας τιμές στα lowlink και index κάθε κόμβου του γράφου, έτσι ώστε να δημιουργήσει την δομή που περιέχει όλα τα SCC.
    
    
* __Grail__
    
    * `isReachableGrailIndex()`
    Ελέγχει με την βοήθεια του Ευρετηρίου Grail την πιθανότητα εύρεσης μονοπατιύ ανάμεσα σε δύο δοθέντες κόμβους.

### Info

Η εργασία αναπτύχθηκε στο CLion σε περιβάλλοντα Linux και macOS.

__Εντολή μεταγλώττισης:__ `gcc -o Project main.c buffer.c index.c hash.c search.c scc.c grail.c cc.c -lpthread`

__Εντολή εκτέλεσης:__ `./Project <graph file> <queries file>`

__Ενδεικτικοί χρόνοι__ (με `THREAD_POOL_SIZE 8` και `NUM_GRAIL 5`):

* __Medium__
    Static: 5.5 sec
    Dynamic: 6.2 sec

* __Large__
    Static: 1 min 2.3 sec
    Dynamic: 4 min 17.1 sec
    