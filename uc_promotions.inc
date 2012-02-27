<?php
/**
 * @file
 * Promotions administration menu items.
 */

/**
 * Display created promotions.
 */
function uc_promotions_get_promos(){
  $output = t("<p>Here you can find all created promotions:</p>");
  $output .= _get_promos();
  return $output;
}

/**
 * Display a form to create a promotion.
 */
function uc_promotions_create_promo(){
  $output = t("<p>Fill the form to create a new promotion:</p>");
  $output .= drupal_render(drupal_get_form('uc_promotions_form'));
  return $output;
}

/**
 * Display a form to modify a promotion.
 */
function uc_promotions_edit_promo($promo_id = ''){
  $output = t("<p>Fill the form to modify a promotion:</p>");
  $output .= drupal_render(drupal_get_form('uc_promotions_form', $promo_id));
  return $output;
}

/*
 * Returns the form to add a promotion
 */
function uc_promotions_form($form, &$form_state){
  $data = $form_state['build_info']['args'][0];

  $form['nid'] = array(
    '#title' => t('Product'),
    '#description' => t('Attention: to view a product in this list the product should be published'),
    '#type' => 'select',
    '#options' => _get_products(),
    '#required' => true,
    '#multiple' => false,
    '#default_value' => $data['nid'],
  );
  
  $form['price_limit'] = array(
    '#title' => t('Price limit'),
    '#description' => t('Price limit that fires the promotion'),
    '#type' => 'textfield',
    '#required' => true,
    '#size' => 30,
    '#default_value' => $data['price_limit'],
  );
  
  $form['domain'] = array(
    '#title' => t('Domain'),
    '#description' => t('If you have multisite installation you can limit this promotion to fires only for that subdomain'),
    '#type' => 'textfield',
    '#required' => false,
    '#size' => 30,
    '#default_value' => $data['domain'],
  );
  
  $form['status'] = array(
    '#title' => t('Attivo'),
    '#description' => t('The promotion is active'),
    '#type' => 'select',
    '#options' => array('' => '', '0'=>t('no'), '1'=>t('yes')),
    '#required' => true,
    '#default_value' => $data['status'],
  );
  
  $form['promoid'] = array(
    '#title' => t('Promoid'),
    '#type' => 'hidden',
    '#default_value' => $data['promoid'],
  );
  
  if($data['promoid']){
    $form['del'] = array(
      '#title' => t('Delete'),
      '#description' => t('Delete the promotion'),
      '#type' => 'select',
      '#options' => array('0'=>t('no'), '1'=>t('yes')),
      '#required' => false,
    );
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Invia i dati'),
  );
  
  return $form;
}

/*
 * Validates form values 
 */
function uc_promotions_form_validate($form, &$form_state){
  if(!is_numeric($form_state['values']['price_limit'])){
    form_set_error('', t("Price limit require only numerical values"));
  }
}

/*
 * Submits form values 
 */
function uc_promotions_form_submit($form, &$form_state){
  $v = $form_state['values'];
  
  //Delete a promotion
  if($v['del'] == 1){
    $query = db_delete('uc_promotions')->condition('promoid', $v['promoid']);
  
    if($query->execute()){
      $form_state['redirect'] = 'admin/store/uc_promotions';
      drupal_set_message(t('Promotion deleted'), $type = 'status');
    }
  }else{
    //Create or update a promotion
    $update = isset($v['promoid']) && $v['promoid'] != '' ? 'promoid' : array();

    if(drupal_write_record('uc_promotions', $form_state['values'], $update)) {
      $form_state['redirect'] = 'admin/store/uc_promotions';
      drupal_set_message(t('Promotion created/modified'), $type = 'status');
    }else{
      drupal_set_message(t('An error has occured while processing the request'), $type = 'error');
    }
  }
}


/* 
 * Returns a list of promotions
 */
function _get_promos(){
  $query = db_select('uc_promotions', 'p');
  $query->fields('p', array('promoid', 'nid', 'price_limit', 'domain', 'status'));
  $results = $query->execute();  
  
  $header = array(
    array('data' => t('Promo-id')),
    array('data' => t('Product')),
    array('data' => t('Price limit')),
    array('data' => t('Domain')),
    array('data' => t('Status')),
    array('data' => t('Actions')),
  );
  
  $rows = array();

  foreach($results as $data){
    $product = node_load($data->nid);
    
    $rows[] = array(
      array('data' => $data->promoid),
      array('data' => l($product->title, 'node/'.$product->nid)),
      array('data' => '&euro; '.number_format($data->price_limit, 2)),
      array('data' => $data->domain),
      array('data' => $data->status == '1' ? t('active') : t('not active')),
      array('data' => l(t('edit'), 'admin/store/uc_promotions/'.$data->promoid.'/edit')),
    );
  }
   
  return theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'quotation-table')));

}

/*
 * Returns active products
 */
function _get_products(){
  $query = db_select('node', 'n')
            ->fields('n', array('nid', 'title'))
            ->condition('type', 'product')
            ->condition('status', 1);
  $results = $query->execute();

  $rows = array();
  foreach($results as $data){
    $rows[$data->nid] = '['.$data->nid.'] '.$data->title;
  }

  return $rows;
}